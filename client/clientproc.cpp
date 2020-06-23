#include "clientproc.h"
#include <qtextcodec.h>
#include <QMessageBox>
#include <QtWidgets/QApplication>

typedef std::deque<Message> message_queue;

ClientProc::ClientProc()
        : work_(new boost::asio::io_context::work(io_context_)),
          resolver_(io_context_),
          socket_(io_context_),
          username_(""),
          mail_(""),
          color_("#00ffffff"),
          fullBody(""),
          crdt() {
            worker_= std::thread([&](){
                io_context_.run(); //boost thread loop start
          });
    do_connect();
}

ClientProc::~ClientProc() {
    work_.reset();
    this->close();
    worker_.join();
}

void ClientProc::do_connect() {
    auto endpoints = resolver_.resolve("127.0.0.1", "63505");
    boost::asio::async_connect(socket_, endpoints,
                               [this](boost::system::error_code ec, const tcp::endpoint&) {
       if (!ec) {
           status = true;
           emit statusChanged(status);
           do_read_header();
       } else {
           qDebug() << ec.message().c_str();
           status = false;
           emit statusChanged(status);
       }
   });
}

void ClientProc::do_read_header() {
    memset(read_msg_.data(), 0, read_msg_.length()+1); //VERY IMPORTANT, otherwise rubbish remains inside socket!
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.data(), Message::header_length+1),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
            read_msg_.decode_header();
            do_read_body();
        }
        else {
            qDebug() << ec.message().c_str() << endl;
            closeConnection();
        }
    });
}

void ClientProc::do_read_body() {
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.body()+1, read_msg_.body_length()),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
            read_msg_.data()[read_msg_.length()+1] = '\0';  //VERY IMPORTANT: this removes any possible letters after data
            fullBody.append(read_msg_.body()+1);

            if(read_msg_.isThisLastChunk()=='0') {
                do_read_header();
                return;
            }

            std::string opJSON;
            try {
                json jdata_in = json::parse(fullBody);
                Jsonize::from_json(jdata_in, opJSON);

                if(opJSON == "LOGIN_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "LOGIN_OK") {
                        qDebug() << "Login success" << endl;
                        std::string db_usernameLoginJSON;
                        std::string db_colorJSON;
                        std::string db_mailJSON;
                        int idJSON;
                        Jsonize::from_json_usernameLogin(jdata_in, db_usernameLoginJSON, db_colorJSON, db_mailJSON, idJSON);
                        QString name_qstring = QString::fromUtf8(db_usernameLoginJSON.data(), db_usernameLoginJSON.size()); //convert to QString
                        QString color_qstring = QString::fromUtf8(db_colorJSON.data(), db_colorJSON.size());
                        QString mail_qstring = QString::fromUtf8(db_mailJSON.data(), db_mailJSON.size());

                        this->setUsername(name_qstring);
                        this->setColor(color_qstring);
                        this->setMail(mail_qstring);
                        this->crdt.setSiteId(idJSON);


                        emit formResultSuccess("LOGIN_SUCCESS");
                    } else {
                        qDebug() << "Wrong user or password" << endl;
                        emit formResultFailure("LOGIN_FAILURE");
                    }
                } else if(opJSON == "SIGNUP_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "SIGNUP_OK")
                        emit formResultSuccess("SIGNUP_SUCCESS");
                    else
                        emit formResultFailure("SIGNUP_FAILURE");
                } else if(opJSON == "LOGOUT_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "LOGOUT_OK")
                        emit opResultSuccess("LOGOUT_SUCCESS");
                    else
                        emit opResultFailure("LOGOUT_FAILURE");
                } else if(opJSON == "DISCONNECT_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "LOGOUT_OK")
                        emit opResultSuccess("DISCONNECT_SUCCESS");
                    else
                        emit opResultFailure("DISCONNECT_FAILURE");
                } else if(opJSON == "GET_USER_OFFLINE_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_collab_colors_map(jdata_in, db_responseJSON);

                    if(db_responseJSON == "GET_USER_OFFLINE_OK") {
                        std::string db_usernameJSON;
                        myCollabColorsMap db_mapJSON;
                        Jsonize::from_json_user_offline(jdata_in, db_usernameJSON, db_mapJSON);
                        emit removeRemoteCursor(db_usernameJSON);
                        emit getUserOffline(db_mapJSON);
                    } else {
                        qDebug() << "Something went wrong with db" << endl;
                    }
                } else if(opJSON == "GET_USER_ONLINE_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_collab_colors_map(jdata_in, db_responseJSON);

                    if(db_responseJSON == "GET_USER_ONLINE_OK") {
                        std::string db_usernameJSON;
                        myCollabColorsMap db_mapJSON;
                        Jsonize::from_json_user_offline(jdata_in, db_usernameJSON, db_mapJSON);
                        emit getUserOnline(db_mapJSON);
                    } else {
                        qDebug() << "Something went wrong with db" << endl;
                    }
                } else if(opJSON == "LOGOUTURI_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "LOGOUTURI_OK") {
                        emit editorResultSuccess("LOGOUTURI_SUCCESS");
                        emit backToMenuWindow();
                    } else
                        emit editorResultFailure("LOGOUTURI_FAILURE");
                } else if(opJSON == "NEWFILE_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "NEWFILE_OK") {
                        std::string uriJSON;
                        Jsonize::from_jsonUri(jdata_in, uriJSON); //get json value and put into JSON variables
                        QString uriQString = QString::fromUtf8(uriJSON.data(), uriJSON.size());

                        //Update client data
                        this->setFileURI(uriQString);
                        this->crdt.setSymbols(std::vector<Symbol>());
                        emit opResultSuccess("NEWFILE_SUCCESS");
                    } else
                        emit opResultFailure("NEWFILE_FAILURE");
                } else if(opJSON == "OPENFILE_RESPONSE") {

                    auto t_start1 = std::chrono::high_resolution_clock::now();

                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "OPENFILE_OK") {
                        std::vector<Symbol> symbolsJSON;
                        Jsonize::from_json_symbols(jdata_in, symbolsJSON);

                        //Update client data
                        this->crdt.setSymbols(symbolsJSON);

                        emit opResultSuccess("OPENFILE_SUCCESS");
                    } else if(db_responseJSON == "OPENFILE_FILE_EMPTY") {
                        //Update client data
                        this->crdt.setSymbols(std::vector<Symbol>());
                        emit opResultSuccess("OPENFILE_SUCCESS");
                    } else
                        emit opResultFailure("OPENFILE_FAILURE");
                } else if(opJSON == "RENAMEFILE_RESPONSE") {
                    std::string db_responseJSON;
                    std::string filenameJSON;
                    Jsonize::from_json_rename_file(jdata_in, db_responseJSON, filenameJSON);

                    if(db_responseJSON == "RENAME_OK")
                        emit editorResultSuccess("RENAME_SUCCESS", filenameJSON.c_str());
                    else
                        emit editorResultFailure("RENAME_FAILURE");
                } else if(opJSON == "OPENWITHURI_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "OPENWITHURI_OK") {
                        std::vector<Symbol> symbolsJSON;
                        std::string filenameJSON;
                        Jsonize::from_json_symbolsAndFilename(jdata_in, symbolsJSON, filenameJSON);

                        //Update client data
                        this->setFilename(QString::fromStdString(filenameJSON));
                        this->crdt.setSymbols(symbolsJSON);

                        qDebug() << "OPENWITHURI success" << endl;
                        emit opResultSuccess("OPENWITHURI_SUCCESS");
                    } else if(db_responseJSON == "OPENFILE_FILE_EMPTY") {
                        std::string filenameJSON;
                        Jsonize::from_json_filename(jdata_in, filenameJSON);

                        //Update client data
                        this->setFilename(QString::fromStdString(filenameJSON));
                        this->crdt.setSymbols(std::vector<Symbol>());
                        emit opResultSuccess("OPENFILE_SUCCESS");
                    } else {
                        qDebug() << "Something went wrong" << endl;
                        emit opResultFailure("OPENWITHURI_FAILURE");
                    }
                } else if(opJSON == "LISTFILE_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_resp(jdata_in, db_responseJSON);

                    if(db_responseJSON == "LIST_EXIST") {
                        std::vector<json> jsonFiles;
                        Jsonize::from_json_files(jdata_in, jsonFiles);

                        std::vector<File> files;
                        for(const auto& j: jsonFiles) {
                            File *f = nullptr; //do not remember to delete it! (keyword 'delete')
                            f = Jsonize::from_json_file(j);
                            if(f==nullptr) {
                                emitMsgInCorrectWindow(); //show error
                                do_read_header();
                            }
                            files.push_back(*f);
                            delete f;
                        }
                        emit listFileResult(files);

                        qDebug() << "Listfile success" << endl;
                        emit opResultSuccess("LISTFILE_SUCCESS");
                    } else if (db_responseJSON == "LIST_DOESNT_EXIST"){
                        qDebug() << "Non ha nessuna lista di file" << endl;
                        emit opResultFailure("LISTFILE_FAILURE_LISTNOTEXIST");
                    } else {
                        qDebug() << "Something went wrong" << endl;
                        emit opResultFailure("LISTFILE_FAILURE");
                    }
                } else if(opJSON == "INVITE_URI_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_inviteURI(jdata_in, db_responseJSON);

                    if(db_responseJSON == "INVITE_URI_SUCCESS")
                        emit editorResultSuccess("INVITE_URI_SUCCESS");
                    else if(db_responseJSON == "ALREADY_PARTECIPANT")
                        emit editorResultFailure("ALREADY_PARTECIPANT");
                    else if(db_responseJSON == "INVITED_NOT_EXIST")
                        emit editorResultFailure("INVITED_NOT_EXIST");
                    else if(db_responseJSON == "SAME_USER")
                        emit editorResultFailure("SAME_USER");
                    else if(db_responseJSON == "SEND_EMAIL_FAILED")
                        emit editorResultFailure("SEND_EMAIL_FAILED");
                    else
                        emit editorResultFailure("INVITE_URI_FAILURE");
                } else if(opJSON == "INSERTION_RESPONSE") {
                    Symbol symbolJSON;
                    int indexEditorJSON;
                    Jsonize::from_json_insertion(jdata_in, symbolJSON, indexEditorJSON);

                    //process received symbol and retrieve new calculated index
                    int newIndex = this->crdt.process(0, indexEditorJSON, symbolJSON);

                    std::pair<int, wchar_t> tuple = std::make_pair(newIndex, symbolJSON.getLetter());
                    emit insertSymbol(tuple, symbolJSON.getStyle());
                } else if(opJSON == "INSERTIONRANGE_RESPONSE") {
                    int firstIndexJSON;
                    std::vector<json> jsonSymbols;
                    Jsonize::from_json_insertion_range(jdata_in, firstIndexJSON, jsonSymbols);
                    std::vector<Symbol> symbols;
                    int newIndex = firstIndexJSON;

                    //generate symbols vector from json vector
                    for(const auto& j: jsonSymbols) {
                        Symbol *s = nullptr; //do not remember to delete it! (keyword 'delete')
                        s = Jsonize::from_json_symbol(j);
                        if(s==nullptr) {
                            emitMsgInCorrectWindow(); //show error
                            do_read_header();
                        }
                        symbols.push_back(*s);

                        //process received symbol and retrieve new calculated index
                        newIndex = this->crdt.process(0, newIndex, *s);

                        std::pair<int, wchar_t> tuple = std::make_pair(newIndex, s->getLetter());
                        emit insertSymbol(tuple, s->getStyle());

                        delete s;
                    }
                } else if(opJSON == "CURSOR_CHANGE_RESPONSE") {
                    std::string usernameJSON;
                    std::string colorJSON;
                    int posJSON;
                    Jsonize::from_json_cursor_change(jdata_in, usernameJSON, colorJSON, posJSON);
                    emit changeRemoteCursor(usernameJSON, colorJSON, posJSON);
                } else if(opJSON == "COLLAB_COLORS_RESPONSE") {
                    std::string db_responseJSON;
                    Jsonize::from_json_collab_colors_map(jdata_in, db_responseJSON);

                    if(db_responseJSON == "COLLAB_COLORS_MAP_OK") {
                        myCollabColorsMap collabColorsMapJSON;
                        Jsonize::from_json_collab_colors_resp(jdata_in, collabColorsMapJSON);
                        for (const auto& kv : collabColorsMapJSON)
                            qDebug() << "username: " << QString::fromStdString(kv.first) << " color: " << QString::fromStdString(kv.second.first) << " isOnline: " << kv.second.second;
                        emit showCollabColorsMap(collabColorsMapJSON);
                    }
                } else if(opJSON == "REMOVAL_RESPONSE") {
                    std::vector<sId> symbolsId;
                    Jsonize::from_json_removal_range(jdata_in, symbolsId);

                    int newIndex;
                    for(const sId& id : symbolsId) {
                        //process received symbol and retrieve new calculated index
                        newIndex = this->crdt.processErase(id);
                        if(newIndex != -1) {
                            emit eraseSymbols(newIndex, newIndex+1);
                        }
                    }
                } else if(opJSON == "FORMAT_RANGE_RESPONSE") {
                    std::vector<sId> symbolsId;
                    int formatJSON;
                    Jsonize::from_json_format_range(jdata_in, symbolsId, formatJSON);
                    int newIndex;
                    for(const sId& id : symbolsId) {
                        //process received symbol and retrieve new calculated index
                        newIndex = this->crdt.processFormat(id, formatJSON);
                        if(newIndex != -1) {
                            emit formatSymbols(newIndex, newIndex+1, formatJSON);
                        }
                    }
                } else if(opJSON == "FONTSIZE_CHANGE_RESPONSE") {
                    std::vector<sId> symbolsId;
                    int fontSizeJSON;
                    Jsonize::from_json_fontsize_change(jdata_in, symbolsId, fontSizeJSON);
                    int newIndex;
                    for(const sId& id : symbolsId) {
                        //process received symbol and retrieve new calculated index
                        newIndex = this->crdt.processFontSize(id, fontSizeJSON);
                        if(newIndex != -1) {
                            emit changeFontSize(newIndex, newIndex+1, fontSizeJSON);
                        }
                    }
                } else if(opJSON == "FONTFAMILY_CHANGE_RESPONSE") {
                    std::vector<sId> symbolsId;
                    std::string fontFamilyJSON;
                    Jsonize::from_json_fontfamily_change(jdata_in, symbolsId, fontFamilyJSON);
                    int newIndex;
                    for(const sId& id : symbolsId) {
                        //process received symbol and retrieve new calculated index
                        newIndex = this->crdt.processFontFamily(id, fontFamilyJSON);
                        if(newIndex != -1) {
                            emit changeFontFamily(newIndex, newIndex+1, fontFamilyJSON);
                        }
                    }
                } else if(opJSON == "ALIGNMENT_CHANGE_RESPONSE") {
                    std::vector<sId> symbolsId;
                    int alignmentJSON;
                    Jsonize::from_json_alignment_change(jdata_in, symbolsId, alignmentJSON);
                    int newIndex;

                    if(symbolsId.empty())
                        emit changeAlignment(newIndex, newIndex+1, alignmentJSON);
                    else {
                        for(const sId& id : symbolsId) {
                            //process received symbol and retrieve new calculated index
                            newIndex = this->crdt.processAlignment(id, alignmentJSON);
                            if(newIndex != -1) {
                                emit changeAlignment(newIndex, newIndex+1, alignmentJSON);
                            }
                        }
                    }
                } else {
                    qDebug() << "Something went wrong" << endl;
                    emit opResultFailure("RESPONSE_FAILURE");
                }
                fullBody = "";
                do_read_header(); //continue reading loop
            } catch (json::exception& e) {
                std::cerr << "message: " << e.what() << '\n' << "exception id: " << e.id << std::endl;
                emitMsgInCorrectWindow();
                fullBody = "";
                do_read_header();
            }
        }
        else {
            qDebug() << ec.message().c_str() << endl;
            closeConnection();
        }
    });

}

void ClientProc::emitMsgInCorrectWindow(){
    emit jsonMsgFailure("StartWindow","Si è verificato un errore nel parsing del json.");
}

void ClientProc::do_write() {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()+1),
                             [this](boost::system::error_code ec, std::size_t /*length*/) {
         if (!ec) {
             qDebug() << "Sent:" << write_msgs_.front().data() << "END" << endl;
             write_msgs_.pop_front();
             if (!write_msgs_.empty()) {
                 do_write();
             }
         }
         else {
             qDebug() << ec.message().c_str() << endl;
             closeConnection();
         }
     });
}

bool ClientProc::getStatus() {
    return status;
}

void ClientProc::closeConnection() {
    status = false;
    emit statusChanged(status);
    socket_.close();
}

void ClientProc::write(const Message& msg) {
    boost::asio::post(io_context_, [this, msg]() {
      bool write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if (!write_in_progress) {
          do_write();
      }
  });
}

void ClientProc::close() {
    boost::asio::post(io_context_, [this]() {
        closeConnection();
    });
}

void ClientProc::setUsername(QString name) {
    this->username_ = name;
}

void ClientProc::setMail(QString mail) {
    this->mail_ = mail;
}

void ClientProc::setColor(QString color) {
    this->color_ = color;
}

QString ClientProc::getUsername() {
    return this->username_;
}

QString ClientProc::getMail() {
    return this->mail_;
}

QString ClientProc::getColor() {
    return this->color_;
}

void ClientProc::setVectorFile(std::vector<File> files){
    this->fileVector_ = files;
}

std::vector<File> ClientProc::getVectorFile(){
    return this->fileVector_;
}

void ClientProc::setFilename(QString filename) {
    this->filename_ = filename;
}

QString ClientProc::getFilename() {
    return this->filename_;
}

void ClientProc::setFileURI(QString uri) {
    this->uri_ = uri;
}

QString ClientProc::getFileURI() {
    return this->uri_;
}

void ClientProc::sendRequestMsg(std::string request) {
    int mod = (request.length()%MAX_CHUNK_LENGTH==0) ? 1 : 0;
    int numChanks = (int)((request.length() / MAX_CHUNK_LENGTH) + 1 - mod);
    int chunkSize = MAX_CHUNK_LENGTH;
    char isLastChunk = '0';
    std::string chunkResponse = request;
    for(int i=0; i<numChanks; i++) {
        if(i == numChanks-1) {
            chunkSize = (int)(request.length() % MAX_CHUNK_LENGTH);
            isLastChunk = '1';
        }
        Message msg = Message::constructMsg(std::string(chunkResponse.begin(), chunkResponse.begin() + chunkSize), isLastChunk);
        chunkResponse.erase(0, chunkSize);
        this->write(msg); //deliver msg to the server
    }
}

