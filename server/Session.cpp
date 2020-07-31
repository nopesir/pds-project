//
// Created by gheb on 24/04/20.
//

#include <boost/asio.hpp>
#include <iostream>
#include <QtSql/QSqlDatabase>
//#include "sqlite3.h"
#include "Session.h"
#include "SharedEditor.h"

Session::Session(boost::asio::ip::tcp::socket socket)
        :_socket(std::move(socket)){}

void Session::session_start(int editorId) {
    shared_from_this()->setSiteId(editorId);
    shared_from_this()->setCurrentFile("");
    fullBody = "";
    SharedEditor::getInstance().join(shared_from_this());
    do_read_header();
}

void Session::deliver(const Message &msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

void Session::do_read_header()
{
    memset(read_msg_.data(), 0, read_msg_.length()+1); //VERY IMPORTANT, otherwise rubbish remains inside socket!
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(read_msg_.data(), Message::header_length+1),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    read_msg_.decode_header();
                                    do_read_body();
                                }
                                else {
                                    //Disconnect user and leave the room
                                    if(!shared_from_this()->getUsername().empty()) { //it can be empty at the beginning (if a crash happens)
                                        dbService::tryLogout(shared_from_this()->getUsername());
                                        QSqlDatabase::removeDatabase("MyConnect2");
                                    }
                                    SharedEditor::getInstance().leave(shared_from_this());
                                }
                            });
}

void Session::do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(read_msg_.body()+1, read_msg_.body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    read_msg_.data()[read_msg_.length()+1] = '\0';  //VERY IMPORTANT: this removes any possible letters after data
                                    fullBody.append(read_msg_.body()+1);

                                    if(read_msg_.isThisLastChunk()=='0') {
                                        do_read_header();
                                        return;
                                    }
                                    //std::cout << "read msg:" << fullBody << "END" << std::endl;

                                    std::string opJSON;
                                    json jdata_in;
                                    try {
                                        jdata_in = json::parse(fullBody);
                                        jsonUtility::from_json(jdata_in, opJSON); //get json value and put into JSON variables

                                        int edId = shared_from_this()->getId();
                                        std::string curFile = std::string();
                                        bool onlyToThisEditor = false;
                                        const std::string response = this->handleRequests(opJSON, jdata_in, edId, curFile, onlyToThisEditor);
                                        if(opJSON == "INSERTION_REQUEST" || opJSON == "REMOVAL_REQUEST" || opJSON == "INSERTIONRANGE_REQUEST" ||
                                           opJSON == "FORMAT_RANGE_REQUEST" || opJSON == "FONTSIZE_CHANGE_REQUEST" || opJSON == "FONTFAMILY_CHANGE_REQUEST" ||
                                           opJSON == "ALIGNMENT_CHANGE_REQUEST" || opJSON == "CURSOR_CHANGE_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->sendMsgAll(response, edId, curFile); //send data to all the participants in the room except to this client, having the curFile opened
                                        }
                                        else if((opJSON == "RENAMEFILE_REQUEST" || opJSON == "COLLAB_COLORS_REQUEST") && !onlyToThisEditor) {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->sendMsgAll(response, edId, curFile, true); //send data to all the participants, having the curFile opened
                                        }
                                        else if(opJSON == "LOGOUTURI_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->sendMsg(response); //send data only to this participant

                                            if (response.find("LOGOUTURI_OK") != std::string::npos) {
                                                const char *db_res;
                                                std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
                                                dbService::DB_RESPONSE resp = dbService::tryGetCollabColors(curFile, mapCollabColors);
                                                QSqlDatabase::removeDatabase("MyConnect");

                                                if (resp == dbService::GET_COLLAB_COLORS_MAP_OK)
                                                    db_res = "GET_USER_OFFLINE_OK";
                                                else if(resp == dbService::DB_ERROR)
                                                    db_res = "DB_ERROR";
                                                else if(resp == dbService::QUERY_ERROR)
                                                    db_res = "QUERY_ERROR";
                                                else
                                                    db_res = "DB_ERROR";

                                                json j;
                                                jsonUtility::to_json_user_offline(j, "GET_USER_OFFLINE_RESPONSE", db_res, shared_from_this()->getUsername(), mapCollabColors);
                                                const std::string response2 = j.dump();
                                                std::cout << "Sent:" << response2 << "END" << std::endl;
                                                this->sendMsgAll(response2, edId, curFile); //send data to all the participants, having the curFile opened
                                            }
                                        }
                                        else if(opJSON == "OPENFILE_REQUEST" || opJSON == "OPENWITHURI_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->sendMsg(response); //send data only to this participant

                                            if (response.find("OPENFILE_FILE_EMPTY") != std::string::npos || response.find("OPENFILE_OK") != std::string::npos ||
                                                response.find("OPENWITHURI_OK") != std::string::npos) {
                                                const char *db_res;
                                                std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
                                                dbService::DB_RESPONSE resp = dbService::tryGetCollabColors(shared_from_this()->getCurrentFile(), mapCollabColors);
                                                QSqlDatabase::removeDatabase("MyConnect");

                                                if (resp == dbService::GET_COLLAB_COLORS_MAP_OK)
                                                    db_res = "GET_USER_ONLINE_OK";
                                                else if(resp == dbService::DB_ERROR)
                                                    db_res = "DB_ERROR";
                                                else if(resp == dbService::QUERY_ERROR)
                                                    db_res = "QUERY_ERROR";
                                                else
                                                    db_res = "DB_ERROR";

                                                json j;
                                                jsonUtility::to_json_user_offline(j, "GET_USER_ONLINE_RESPONSE", db_res, shared_from_this()->getUsername(), mapCollabColors);
                                                const std::string response2 = j.dump();
                                                std::cout << "Sent:" << response2 << "END" << std::endl;
                                                this->sendMsgAll(response2, edId, curFile); //send data to all the participants, having the curFile opened
                                            }
                                        }
                                        else {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->sendMsg(response); //send data only to this participant
                                        }
                                        fullBody.clear();
                                        do_read_header(); //continue reading loop
                                    } catch (json::exception& e) {
                                        std::cerr << "message: " << e.what() << '\n' << "exception id: " << e.id << std::endl;
                                        fullBody = "";
                                        do_read_header();
                                    }
                                }
                                else {
                                    //Disconnect user and leave the room
                                    if(!shared_from_this()->getUsername().empty()) { //it can be empty at the beginning (if a crash happens)
                                        dbService::tryLogout(shared_from_this()->getUsername());
                                        QSqlDatabase::removeDatabase("MyConnect2");
                                    }
                                    SharedEditor::getInstance().leave(shared_from_this());
                                }
                            });
}

void Session::do_write() {
    //std::this_thread::sleep_for (std::chrono::seconds(2));
    auto self(shared_from_this());
    boost::asio::async_write(_socket,
                             boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()+1),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty()) {
                                         do_write();
                                     }
                                 }
                                 else {
                                     //Disconnect user and leave the room
                                     if(!shared_from_this()->getUsername().empty()) { //it can be empty at the beginning (if a crash happens)
                                         dbService::tryLogout(shared_from_this()->getUsername());
                                         QSqlDatabase::removeDatabase("MyConnect2");
                                     }
                                     SharedEditor::getInstance().leave(shared_from_this());
                                 }
                             });
}

void Session::sendMsg(const std::string& response) {
    int mod = (response.length()%MAX_CHUNK_LENGTH==0) ? 1 : 0;
    int numChanks = (int)((response.length() / MAX_CHUNK_LENGTH) + 1 - mod);
    int chunkSize = MAX_CHUNK_LENGTH;
    char isLastChunk = '0';
    std::string chunkResponse = response;
    for(int i=0; i<numChanks; i++) {
        if(i == numChanks-1) {
            chunkSize = (int)(response.length() % MAX_CHUNK_LENGTH);
            isLastChunk = '1';
        }
        Message msg = Message::constructMsg(std::string(chunkResponse.begin(), chunkResponse.begin() + chunkSize), isLastChunk);
        chunkResponse.erase(0, chunkSize);
        shared_from_this()->deliver(msg); //deliver msg only to the participant
    }
}

void Session::sendMsgAll(const std::string& response, const int& edId, const std::string& curFile, bool includeThisEditor) {
    int mod = (response.length() % MAX_CHUNK_LENGTH == 0) ? 1 : 0;
    int numChanks = (int) ((response.length() / MAX_CHUNK_LENGTH) + 1 - mod);
    int chunkSize = MAX_CHUNK_LENGTH;
    char isLastChunk = '0';
    std::string chunkResponse = response;
    for (int i = 0; i < numChanks; i++) {
        if (i == numChanks - 1) {
            chunkSize = (int) (response.length() % MAX_CHUNK_LENGTH);
            isLastChunk = '1';
        }
        Message msg = Message::constructMsg(std::string(chunkResponse.begin(), chunkResponse.begin() + chunkSize), isLastChunk);
        chunkResponse.erase(0, chunkSize);
        SharedEditor::getInstance().deliverToAll(msg, edId, curFile, includeThisEditor); //deliver msg to all the clients except the client with id 'edId' (this client)
    }
}

std::string Session::handleRequests(const std::string& opJSON, const json& jdata_in, int& edId, std::string& curFile, bool& onlyToThisEditor) {
    if(opJSON == "LOGIN_REQUEST") {
        std::string userJSON;
        std::string passJSON;
        jsonUtility::from_json(jdata_in, userJSON, passJSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        QString colorJSON = "#00ffffff";
        QString mail = "";
        dbService::DB_RESPONSE resp = dbService::tryLogin(userJSON, passJSON, colorJSON, mail);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::LOGIN_OK) {
            shared_from_this()->setUsername(userJSON);
            shared_from_this()->setColor(colorJSON.toStdString());
            db_res = "LOGIN_OK";
        }
        else if(resp == dbService::LOGIN_FAILED)
            db_res = "LOGIN_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else if(resp == dbService::ALREADY_LOGGED)
            db_res = "ALREADY_LOGGED_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json_usernameLogin(j, "LOGIN_RESPONSE", db_res, userJSON, colorJSON.toStdString(),
                                           mail.toStdString(), shared_from_this()->getId());
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "LOGOUT_REQUEST" || opJSON == "DISCONNECT_REQUEST") {
        std::string userJSON;
        jsonUtility::from_json_username(jdata_in, userJSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::tryLogout(userJSON);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::LOGOUT_OK)
            db_res = "LOGOUT_OK";
        else if(resp == dbService::LOGOUT_FAILED)
            db_res = "LOGOUT_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        json j;
        jsonUtility::to_json(j, opJSON == "LOGOUT_REQUEST" ? "LOGOUT_RESPONSE" : "DISCONNECT_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "LOGOUTURI_REQUEST") {
        std::string userJSON;
        std::string uriJSON;
        jsonUtility::from_json_uri(jdata_in,userJSON, uriJSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::tryLogout(userJSON, uriJSON);
        QSqlDatabase::removeDatabase("MyConnect2");
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened
        edId = shared_from_this()->getId();

        if(resp == dbService::LOGOUT_OK) {
            fileUtility::writeFile(R"(..\Filesystem\)" + uriJSON + ".txt", room::getInstance().getMap().at(uriJSON));
            shared_from_this()->setSymbols(std::vector<Symbol>());
            db_res = "LOGOUTURI_OK";
        }
        else if(resp == dbService::LOGOUT_FAILED)
            db_res = "LOGOUTURI_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        json j;
        jsonUtility::to_json(j, "LOGOUTURI_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "SIGNUP_REQUEST") {
        std::string userJSON;
        std::string passJSON;
        std::string emailJSON;
        jsonUtility::from_json(jdata_in, userJSON, passJSON, emailJSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::trySignup(userJSON, passJSON, emailJSON);
        QSqlDatabase::removeDatabase("MyConnect");

        if(resp == dbService::SIGNUP_OK)
            db_res = "SIGNUP_OK";
        else if(resp == dbService::SIGNUP_FAILED)
            db_res = "SIGNUP_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else if(resp == dbService::EMAIL_ERROR)
            db_res = "EMAIL_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json(j, "SIGNUP_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "NEWFILE_REQUEST") {
        std::string userJSON;
        std::string filenameJSON;

        jsonUtility::from_json_filename(jdata_in, userJSON, filenameJSON); //get json value and put into JSON variables
        filenameJSON = QString::fromUtf8(filenameJSON.c_str()).toLatin1().toStdString(); //conversion needed to save filename string correctly in the DB

        //update tables on db
        const char *db_res;
        QString uri = dbService::generateURI(12);
        dbService::DB_RESPONSE resp = dbService::tryNewFile(userJSON, filenameJSON, uri);
        QSqlDatabase::removeDatabase("MyConnect3");

        //create file on local filesystem
        boost::filesystem::ofstream(R"(..\Filesystem\)" + uri.toStdString() + ".txt");

        if (resp == dbService::NEWFILE_OK) {
            db_res = "NEWFILE_OK";

            //Update session data
            shared_from_this()->setCurrentFile(uri.toStdString());
            SharedEditor::getInstance().addEntryInMap(shared_from_this()->getCurrentFile(), std::vector<symbol>());

            //Serialize data
            json j;
            jsonUtility::to_json_newuri(j, "NEWFILE_RESPONSE", db_res, uri.toStdString());
            const std::string response = j.dump();
            return response;
        }
        else if(resp == dbService::NEWFILE_FAILED)
            db_res = "NEWFILE_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json(j, "NEWFILE_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "LISTFILE_REQUEST") {
        std::string userJSON;
        jsonUtility::from_json_username(jdata_in, userJSON); //get json value and put into JSON variables

        //check the list of file for the current user
        const char *db_res;
        std::vector<File> vectorFile;
        dbService::DB_RESPONSE resp = dbService::tryListFile(userJSON, vectorFile);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::LIST_EXIST) {
            if(SharedEditor::getInstance().getMap().empty()) {
                for (const auto &f: vectorFile)
                    SharedEditor::getInstance().addEntryInMap(f.getidfile(), std::vector<Symbol>());
            } else {
                for (const auto &f: vectorFile)
                    if (SharedEditor::getInstance(). getMap().count(f.getidfile()) <= 0) //key not exists
                        SharedEditor::getInstance().addEntryInMap(f.getidfile(), std::vector<Symbol>());
            }
            db_res = "LIST_EXIST";
        }
        else if(resp == dbService::LIST_DOESNT_EXIST)
            db_res = "LIST_DOESNT_EXIST";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        std::vector<json> fileVectorJSON = jsonUtility::fromFileToJson(vectorFile);
        jsonUtility::to_json_fileVector(j, "LISTFILE_RESPONSE", db_res, fileVectorJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "RENAMEFILE_REQUEST") {
        std::string newNameFileJson;
        std::string uriJson;
        std::string userJSON;
        jsonUtility::from_json_renameFile(jdata_in, newNameFileJson, uriJson,userJSON);

        newNameFileJson = QString::fromUtf8(newNameFileJson.c_str()).toLatin1().toStdString(); //conversion needed to save filename string correctly in the DB

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::tryRenameFile(newNameFileJson, uriJson, userJSON);
        QSqlDatabase::removeDatabase("MyConnect3");
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        if (resp == dbService::RENAME_OK)
            db_res = "RENAME_OK";
        else if(resp == dbService::RENAME_FAILED) {
            db_res = "RENAME_FAILED";
            onlyToThisEditor = true; //the error message popup must appear only to who made the request
        }
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json_rename_file(j, "RENAMEFILE_RESPONSE", db_res, newNameFileJson);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "OPENFILE_REQUEST") {
        std::string userJSON;
        std::string uriJSON;
        jsonUtility::from_json_uri(jdata_in, userJSON, uriJSON); //get json value and put into JSON variables

        //update tables on db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::tryOpenFile(userJSON, uriJSON);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::OPENFILE_OK) {
            //Update session data
            shared_from_this()->setCurrentFile(uriJSON);
            shared_from_this()->setSymbols(SharedEditor::getInstance().getSymbolMap(uriJSON, true));

            SharedEditor::getInstance().updateMap(shared_from_this()->getCurrentFile(),shared_from_this()->getSymbols());

            curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened
            edId = shared_from_this()->getId();

            if(shared_from_this()->getSymbols().empty()) //file is empty (it can happen that one client save an empty file)
                db_res = "OPENFILE_FILE_EMPTY";
            else
                db_res = "OPENFILE_OK";

            //Serialize data
            json j;
            jsonUtility::to_json_symVector(j, "OPENFILE_RESPONSE", db_res, shared_from_this()->getSymbols());
            const std::string response = j.dump();
            return response;
        }
        else if(resp == dbService::OPENFILE_FAILED)
            db_res = "OPENFILE_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json(j, "OPENFILE_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "OPENWITHURI_REQUEST") {
        std::string userJSON;
        std::string uriJSON;
        std::string filenameJSON;
        jsonUtility::from_json_uri(jdata_in, userJSON, uriJSON); //get json value and put into JSON variables

        //update tables on db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::tryOpenWithURIFile(userJSON, uriJSON, filenameJSON);
        QSqlDatabase::removeDatabase("MyConnect2");

        if (resp == dbService::OPENWITHURI_OK) {
            //Update session data
            shared_from_this()->setCurrentFile(uriJSON);
            shared_from_this()->setSymbols(SharedEditor::getInstance().getSymbolMap(uriJSON, true));

            SharedEditor::getInstance().updateMap(shared_from_this()->getCurrentFile(),shared_from_this()->getSymbols());

            curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened
            edId = shared_from_this()->getId();

            if(shared_from_this()->getSymbols().empty()) //file is empty (it can happen that one client save an empty file)
                db_res = "OPENFILE_FILE_EMPTY";
            else
                db_res = "OPENWITHURI_OK";

            //Serialize data
            json j;
            jsonUtility::to_json_symVectorAndFilename(j, "OPENWITHURI_RESPONSE", db_res, shared_from_this()->getSymbols(), filenameJSON);
            const std::string response = j.dump();
            return response;
        }
        else if(resp == dbService::OPENWITHURI_FAILED)
            db_res = "OPENWITHURI_FAILED";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json(j, "OPENWITHURI_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "INVITE_URI_REQUEST") {
        std::string invitedJSON;
        std::string applicantJSON;
        std::string uriJSON;
        jsonUtility::from_json_inviteURI(jdata_in, invitedJSON, applicantJSON, uriJSON); //get json value and put into JSON variables

        // Get email related to that username
        const char *db_res2;
        std::string email_invited;
        dbService::DB_RESPONSE resp2 = dbService::tryGetEmail(invitedJSON, email_invited);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp2 == dbService::GET_EMAIL_OK) {
            // Send email
            if(email::sendEmail(email_invited, uriJSON)) {
                const char *db_res;
                if(invitedJSON == applicantJSON){
                    db_res = "SAME_USER";
                } else {
                    dbService::DB_RESPONSE resp = dbService::tryAddFriend(invitedJSON, uriJSON);
                    QSqlDatabase::removeDatabase("MyConnect2");

                    if(resp == dbService::ALREADY_PARTECIPANT)
                        db_res = "ALREADY_PARTECIPANT";
                    else if(resp == dbService::INVITE_URI_SUCCESS)
                        db_res = "INVITE_URI_SUCCESS";
                    else if(resp == dbService::INVITE_URI_FAILED)
                        db_res = "INVITE_URI_FAILED";
                    else if(resp == dbService::QUERY_ERROR)
                        db_res = "QUERY_ERROR";
                    else if(resp == dbService::INVITED_NOT_EXIST)
                        db_res = "INVITED_NOT_EXIST";
                    else
                        db_res = "DB_ERROR";
                }
                json j;
                jsonUtility::to_json(j, "INVITE_URI_RESPONSE", db_res);
                const std::string response = j.dump();
                return response;
            }
            else {
                const char *db_res = "SEND_EMAIL_FAILED";
                json j;
                jsonUtility::to_json(j, "INVITE_URI_RESPONSE", db_res);
                const std::string response = j.dump();
                return response;
            }
        } else if(resp2 == dbService::INVITED_NOT_EXIST) {
            db_res2 = "INVITED_NOT_EXIST";
        } else if(resp2 == dbService::QUERY_ERROR) {
            db_res2 = "QUERY_ERROR";
        } else {
            db_res2 = "DB_ERROR";
        }
        json j;
        jsonUtility::to_json(j, "INVITE_URI_RESPONSE", db_res2);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "INSERTION_REQUEST") {
        Symbol symbolJSON;
        int indexEditorJSON;
        jsonUtility::from_json_insertion(jdata_in, symbolJSON, indexEditorJSON); //get json value and put into JSON variables
        std::cout << "symbol received: " << symbolJSON.getLetter() << "," << "ID: " << symbolJSON.getId().first << "," << symbolJSON.getId().second << std::endl;

        //process received symbol and retrieve new calculated index
        int newIndex = process(0, indexEditorJSON, SharedEditor::getInstance().getSymbolMap(shared_from_this()->getCurrentFile(),
                                                                                    false), symbolJSON);

        //Update room symbols for this file
        SharedEditor::getInstance().insertInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, symbolJSON);

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_insertion(j, "INSERTION_RESPONSE", symbolJSON, indexEditorJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "REMOVAL_REQUEST") {
        std::vector<std::pair<int, int>> symbolsId;
        jsonUtility::from_json_removal_range(jdata_in, symbolsId);
        int newIndex;

        for(const std::pair<int,int>& id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = getIndexById(SharedEditor::getInstance().getSymbolMap(shared_from_this()->getCurrentFile(),false),id);
            if(newIndex != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().eraseInSymbolMap(shared_from_this()->getCurrentFile(), newIndex);
            }
        }

        //Write on file if there aren't chars (to prevent 2nd client reading from file previous symbols, see 'getSymbolMap', 2nd 'if' statement)
        if(SharedEditor::getInstance().getMap().at(shared_from_this()->getCurrentFile()).empty())
            fileUtility::writeFile(R"(..\Filesystem\)" + shared_from_this()->getCurrentFile() + ".txt",
                                   SharedEditor::getInstance().getMap().at(shared_from_this()->getCurrentFile()));

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_removal_range(j, "REMOVAL_RESPONSE", symbolsId);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "FORMAT_RANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbolsId;
        int formatJSON;
        jsonUtility::from_json_format_range(jdata_in, symbolsId, formatJSON); //get json value and put into JSON variables
        int newIndex;

        for(const std::pair<int, int>& id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = getIndexById(SharedEditor::getInstance().getSymbolMap(
                    shared_from_this()->getCurrentFile(),false), id);
            if(newIndex != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().formatInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, formatJSON);
            }
        }

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send the message only to clients having this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_format_range(j, "FORMAT_RANGE_RESPONSE", symbolsId, formatJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "FONTSIZE_CHANGE_REQUEST") {
        std::vector<std::pair<int, int> symbolsId;
        int fontSizeJSON;
        jsonUtility::from_json_fontsize_change(jdata_in, symbolsId, fontSizeJSON);
        int newIndex;

        for(const std::pair<int, int>& id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = getIndexById(SharedEditor::getInstance().getSymbolMap(shared_from_this()->getCurrentFile(),false), id);
            if(newIndex != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().changeFontSizeInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, fontSizeJSON);
            }
        }

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send the message only to clients having this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_fontsize_change(j, "FONTSIZE_CHANGE_RESPONSE", symbolsId, fontSizeJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "FONTFAMILY_CHANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbolsId;
        std::string fontFamilyJSON;
        jsonUtility::from_json_fontfamily_change(jdata_in, symbolsId, fontFamilyJSON);
        int newIndex;

        for(const std::pair<int, int>& id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = getIndexById(SharedEditor::getInstance().getSymbolMap(shared_from_this()->getCurrentFile(),false), id);
            if(newIndex != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().changeFontFamilyInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, fontFamilyJSON);
            }
        }

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send the message only to clients having this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_fontfamily_change(j, "FONTFAMILY_CHANGE_RESPONSE", symbolsId, fontFamilyJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "ALIGNMENT_CHANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbolsId;
        int alignmentJSON;
        jsonUtility::from_json_alignment_change(jdata_in, symbolsId, alignmentJSON);
        int newIndex = -1;

        for (const std::pair<int, int> &id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = getIndexById(SharedEditor::getInstance().getSymbolMap(shared_from_this()->getCurrentFile(), false), id);
            if (newIndex != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().changeAlignmentInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, alignmentJSON);
            }
        }

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send the message only to clients having this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_alignment_change(j, "ALIGNMENT_CHANGE_RESPONSE", symbolsId, alignmentJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "CURSOR_CHANGE_REQUEST") {
        int posJSON;
        jsonUtility::from_json_cursor_change_req(jdata_in, posJSON);
        std::cout << "pos received: " << std::to_string(posJSON) << std::endl;

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        //Serialize data
        json j;
        jsonUtility::to_json_cursor_change(j, "CURSOR_CHANGE_RESPONSE", shared_from_this()->getUsername(), shared_from_this()->getColor(), posJSON);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "COLLAB_COLORS_REQUEST") {
        std::string uriJSON;
        jsonUtility::from_json_collab_colors(jdata_in, uriJSON);
        std::cout << "uri received: " << uriJSON << std::endl;

        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        const char *db_res;
        std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
        dbService::DB_RESPONSE resp = dbService::tryGetCollabColors(uriJSON, mapCollabColors);
        QSqlDatabase::removeDatabase("MyConnect");

        if (resp == dbService::GET_COLLAB_COLORS_MAP_OK)
            db_res = "COLLAB_COLORS_MAP_OK";
        else if(resp == dbService::DB_ERROR)
            db_res = "DB_ERROR";
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json_collab_colors_resp(j, "COLLAB_COLORS_RESPONSE", db_res, mapCollabColors);
        const std::string response = j.dump();
        return response;

    } else if (opJSON == "INSERTIONRANGE_REQUEST") {
        std::vector<json> formattingSymbolsJSON;
        int startIndexJSON;
        jsonUtility::from_json_insertion_range(jdata_in, formattingSymbolsJSON, startIndexJSON);
        std::vector<Symbol> symbols = jsonUtility::fromJsonToFormattingSym(formattingSymbolsJSON);
        int newIndex = startIndexJSON;

        for(const Symbol& s : symbols) {
            //process received symbol and retrieve new calculated index
            newIndex = process(0, newIndex, SharedEditor::getInstance().getSymbolMap(
                    shared_from_this()->getCurrentFile(),false), s);

            //Update room symbols for this file
            SharedEditor::getInstance().insertInSymbolMap(shared_from_this()->getCurrentFile(), newIndex, s);
        }

        edId = shared_from_this()->getId(); //don't send this message to this editor
        curFile = shared_from_this()->getCurrentFile(); //send only the message to clients that have this currentFile opened

        //Serialize data
        json j;
        std::vector<json> symbolsJSON = jsonUtility::fromSymToJson(symbols);
        jsonUtility::to_json_insertion_range(j, "INSERTIONRANGE_RESPONSE", startIndexJSON, symbolsJSON);
        const std::string response = j.dump();
        return response;

    } else { //editor functions
        SharedEditor::getInstance().deliver(read_msg_); //deliver to all the participants
    }
}
