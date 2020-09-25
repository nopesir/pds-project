//
// Created by gheb on 24/04/20.
//

#include <boost/asio.hpp>
#include <iostream>
#include <QtSql/QSqlDatabase>
#include "Session.h"
#include "SharedEditor.h"
#include "fileUtility.h"
#include "email.h"

using json = nlohmann::json;
Session::Session(boost::asio::ip::tcp::socket socket)
        :_socket(std::move(socket)){}

void Session::start_session(int ed_id) {
    shared_from_this()->set_ed_id(ed_id);
    shared_from_this()->set_curr_file("");
    body = "";
    SharedEditor::getInstance().join(shared_from_this());
    read_header();
}

void Session::deliver(const Message &msg) {
    bool write_in_progress = !msgs_out.empty();
    msgs_out.push_back(msg);
    if (!write_in_progress) {
        write();
    }
}

void Session::read_header()
{
    memset(msg_in.data(), 0, msg_in.length() + 1); //initialization
    auto self(shared_from_this()); //to make sure object outlives async ops
    boost::asio::async_read(_socket,
                            boost::asio::buffer(msg_in.data(), Message::header_length + 1),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    msg_in.decode_header();
                                    read_body();
                                }
                                else {
                                    //Disconnect user and leave the room
                                    if(!shared_from_this()->get_username().empty()) { //it can be empty at the beginning (if a crash happens)
                                        dbService::logout(shared_from_this()->get_username());
                                        QSqlDatabase::removeDatabase("MyConnect2");
                                    }
                                    SharedEditor::getInstance().leave(shared_from_this());
                                }
                            });
}

void Session::read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(msg_in.body() + 1, msg_in.body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    msg_in.data()[msg_in.length() + 1] = '\0';  //VERY IMPORTANT: this removes any possible letters after data
                                    body.append(msg_in.body() + 1);

                                    if(msg_in.is_last_chunk() == '0') {
                                        read_header();
                                        return;
                                    }

                                    std::string j_op;
                                    json j_data;
                                    try {
                                        std::cout << "body of message: " << body << std::endl;
                                        j_data = json::parse(body);
                                        jsonUtility::from_json_simple_op(j_data, j_op); //get json value and put into JSON variables

                                        int edId = shared_from_this()->get_id();
                                        std::string curFile = std::string();
                                        bool onlyToThisEditor = false;
                                        const std::string response = this->process_reqs(j_op, j_data, edId, curFile,
                                                                                        onlyToThisEditor);
                                        if(j_op == "INSERTION_REQUEST" || j_op == "REMOVAL_REQUEST" || j_op == "INSERTIONRANGE_REQUEST" ||
                                           j_op == "FORMAT_RANGE_REQUEST" || j_op == "FONTSIZE_CHANGE_REQUEST" || j_op == "FONTFAMILY_CHANGE_REQUEST" ||
                                           j_op == "ALIGNMENT_CHANGE_REQUEST" || j_op == "CURSOR_CHANGE_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->send_msg_all(response, edId, curFile); //send data to all the participants in the room except to this client, having the curFile opened
                                        }
                                        else if((j_op == "RENAMEFILE_REQUEST" || j_op == "COLLAB_COLORS_REQUEST") && !onlyToThisEditor) {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->send_msg_all(response, edId, curFile, true); //send data to all the participants, having the curFile opened
                                        }
                                        else if(j_op == "LOGOUTURI_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->send_msg(response); //send data only to this participant

                                            if (response.find("LOGOUTURI_OK") != std::string::npos) {
                                                const char *db_res;
                                                std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
                                                dbService::DB_RESPONSE resp = dbService::getColors(curFile,
                                                                                                   mapCollabColors);
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
                                                jsonUtility::to_json_user_on_off(j, "GET_USER_OFFLINE_RESPONSE", db_res,
                                                                                 shared_from_this()->get_username(),
                                                                                 mapCollabColors);
                                                const std::string response2 = j.dump();
                                                std::cout << "Sent:" << response2 << "END" << std::endl;
                                                this->send_msg_all(response2, edId, curFile); //send data to all the participants, having the curFile opened
                                            }
                                        }
                                        else if(j_op == "OPENFILE_REQUEST" || j_op == "OPENWITHURI_REQUEST") {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->send_msg(response); //send data only to this participant

                                            if (response.find("OPENFILE_FILE_EMPTY") != std::string::npos || response.find("OPENFILE_OK") != std::string::npos ||
                                                response.find("OPENWITHURI_OK") != std::string::npos) {
                                                const char *db_res;
                                                std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
                                                dbService::DB_RESPONSE resp = dbService::getColors(
                                                        shared_from_this()->get_curr_file(), mapCollabColors);
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
                                                jsonUtility::to_json_user_on_off(j, "GET_USER_ONLINE_RESPONSE", db_res,
                                                                                 shared_from_this()->get_username(),
                                                                                 mapCollabColors);
                                                const std::string response2 = j.dump();
                                                std::cout << "Sent:" << response2 << "END" << std::endl;
                                                this->send_msg_all(response2, edId, curFile); //send data to all the participants, having the curFile opened
                                            }
                                        }
                                        else {
                                            std::cout << "Sent:" << response << "END" << std::endl;
                                            this->send_msg(response); //send data only to this participant
                                        }
                                        body.clear();
                                        read_header(); //continue reading loop
                                    } catch (json::exception& e) {
                                        std::cerr << "message: " << e.what() << '\n' << "exception id: " << e.id << std::endl;
                                        body = "";
                                        read_header();
                                    }
                                }
                                else {
                                    //Disconnect user and leave the room
                                    if(!shared_from_this()->get_username().empty()) { //it can be empty at the beginning (if a crash happens)
                                        dbService::logout(shared_from_this()->get_username());
                                        QSqlDatabase::removeDatabase("MyConnect2");
                                    }
                                    SharedEditor::getInstance().leave(shared_from_this());
                                }
                            });
}

void Session::write() {
    //std::this_thread::sleep_for (std::chrono::seconds(2));
    auto self(shared_from_this());
    boost::asio::async_write(_socket,
                             boost::asio::buffer(msgs_out.front().data(), msgs_out.front().length() + 1),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     msgs_out.pop_front();
                                     if (!msgs_out.empty()) {
                                         write();
                                     }
                                 }
                                 else {
                                     //Disconnect user and leave the room
                                     if(!shared_from_this()->get_username().empty()) { //it can be empty at the beginning (if a crash happens)
                                         dbService::logout(shared_from_this()->get_username());
                                         QSqlDatabase::removeDatabase("MyConnect2");
                                     }
                                     SharedEditor::getInstance().leave(shared_from_this());
                                 }
                             });
}

void Session::send_msg(const std::string& response) {
    int mod = (response.length()%MAX_CHUNK_LENGTH==0) ? 1 : 0;
    int num_chunks = (int)((response.length() / MAX_CHUNK_LENGTH) + 1 - mod);
    int chunk_sz = MAX_CHUNK_LENGTH;
    char is_last = '0';
    std::string chunkResponse = response;
    for(int i=0; i < num_chunks; i++) {
        if(i == num_chunks - 1) {
            chunk_sz = (int)(response.length() % MAX_CHUNK_LENGTH);
            is_last = '1';
        }
        Message msg = Message::build_msg(std::string(chunkResponse.begin(), chunkResponse.begin() + chunk_sz), is_last);
        chunkResponse.erase(0, chunk_sz);
        shared_from_this()->deliver(msg); //deliver msg only to the participant
    }
}

void Session::send_msg_all(const std::string& response, const int& ed_id, const std::string& curr_file, bool include_this_ed) {
    int mod = (response.length() % MAX_CHUNK_LENGTH == 0) ? 1 : 0;
    int num_chunks = (int) ((response.length() / MAX_CHUNK_LENGTH) + 1 - mod);
    int chunk_sz = MAX_CHUNK_LENGTH;
    char is_last = '0';
    std::string chunk_resp = response;
    for (int i = 0; i < num_chunks; i++) {
        if (i == num_chunks - 1) {
            chunk_sz = (int) (response.length() % MAX_CHUNK_LENGTH);
            is_last = '1';
        }
        Message msg = Message::build_msg(std::string(chunk_resp.begin(), chunk_resp.begin() + chunk_sz), is_last);
        chunk_resp.erase(0, chunk_sz);
        SharedEditor::getInstance().deliver_to_all(msg, ed_id, curr_file, include_this_ed); //deliver msg to all the clients except the client with id 'ed_id' (this client)
    }
}

std::string Session::process_reqs(const std::string& j_op, const json& j_data, int& ed_id, std::string& curr_file, bool& only_this_ed) {
    if(j_op == "LOGIN_REQUEST") {
        std::string user_JSON;
        std::string pass_JSON;
        jsonUtility::from_json(j_data, user_JSON, pass_JSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        QString color_JSON = "#00ffffff";
        QString mail = "";
        dbService::DB_RESPONSE resp = dbService::login(user_JSON, pass_JSON, color_JSON, mail);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::LOGIN_OK) {
            shared_from_this()->set_username(user_JSON);
            shared_from_this()->set_color(color_JSON.toStdString());
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
        jsonUtility::to_json_username_login(j, "LOGIN_RESPONSE", db_res, user_JSON, color_JSON.toStdString(),
                                            mail.toStdString(), shared_from_this()->get_id());
        const std::string response = j.dump();
        return response;

    } else if (j_op == "LOGOUT_REQUEST" || j_op == "DISCONNECT_REQUEST") {
        std::string user_JSON;
        jsonUtility::from_json_username(j_data, user_JSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::logout(user_JSON);
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
        jsonUtility::to_json_simple_resp(j, j_op == "LOGOUT_REQUEST" ? "LOGOUT_RESPONSE" : "DISCONNECT_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "LOGOUTURI_REQUEST") {
        std::string user_JSON;
        std::string uri_JSON;
        jsonUtility::from_json_uri(j_data, user_JSON, uri_JSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::logout(user_JSON, uri_JSON);
        QSqlDatabase::removeDatabase("MyConnect2");
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened
        ed_id = shared_from_this()->get_id();

        if(resp == dbService::LOGOUT_OK) {
            fileUtility::writeFile(R"(../Filesystem/)" + uri_JSON + ".txt", SharedEditor::getInstance().get_map().at(uri_JSON));
            shared_from_this()->set_symbols(std::vector<Symbol>());
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
        jsonUtility::to_json_simple_resp(j, "LOGOUTURI_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "SIGNUP_REQUEST") {
        std::string user_JSON;
        std::string pass_JSON;
        std::string email_JSON;
        jsonUtility::from_json(j_data, user_JSON, pass_JSON, email_JSON); //get json value and put into JSON variables

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::signup(user_JSON, pass_JSON, email_JSON);
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
        jsonUtility::to_json_simple_resp(j, "SIGNUP_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "NEWFILE_REQUEST") {
        std::string user_JSON;
        std::string filename_JSON;

        jsonUtility::from_json_user_filename(j_data, user_JSON, filename_JSON); //get json value and put into JSON variables
        filename_JSON = QString::fromUtf8(filename_JSON.c_str()).toLatin1().toStdString(); //conversion needed to save filename string correctly in the DB

        //update tables on db
        const char *db_res;
        QString uri = dbService::generateURI(12);
        dbService::DB_RESPONSE resp = dbService::newFile(user_JSON, filename_JSON, uri);
        QSqlDatabase::removeDatabase("MyConnect3");

        //create file on local filesystem
       boost::filesystem::ofstream(R"(../Filesystem/)" + uri.toStdString() + ".txt");

        if (resp == dbService::NEWFILE_OK) {
            db_res = "NEWFILE_OK";

            //Update session data
            shared_from_this()->set_curr_file(uri.toStdString());
            SharedEditor::getInstance().add_file(shared_from_this()->get_curr_file(), std::vector<Symbol>());

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
        jsonUtility::to_json_simple_resp(j, "NEWFILE_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "LISTFILE_REQUEST") {
        std::string user_JSON;
        jsonUtility::from_json_username(j_data, user_JSON); //get json value and put into JSON variables

        //check the list of file for the current user
        const char *db_res;
        std::vector<File> vectorFile;
        dbService::DB_RESPONSE resp = dbService::listFile(user_JSON, vectorFile);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::LIST_EXIST) {
            if(SharedEditor::getInstance().get_map().empty()) {
                for (const auto &f: vectorFile)
                    SharedEditor::getInstance().add_file(f.getIdFile(), std::vector<Symbol>());
            } else {
                for (const auto &f: vectorFile)
                    if (SharedEditor::getInstance().get_map().count(f.getIdFile()) <= 0) //key not exists
                        SharedEditor::getInstance().add_file(f.getIdFile(), std::vector<Symbol>());
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
        std::vector<json> fileVectorJSON = jsonUtility::from_files_to_json(vectorFile);
        jsonUtility::to_json_j_file_vector(j, "LISTFILE_RESPONSE", db_res, fileVectorJSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "RENAMEFILE_REQUEST") {
        std::string new_file_name_JSON;
        std::string uri_JSON;
        std::string user_JSON;
        jsonUtility::from_json_rename_file(j_data, new_file_name_JSON, uri_JSON, user_JSON);

        new_file_name_JSON = QString::fromUtf8(new_file_name_JSON.c_str()).toLatin1().toStdString(); //conversion needed to save filename string correctly in the DB

        //Get data from db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::renameFile(new_file_name_JSON, uri_JSON, user_JSON);
        QSqlDatabase::removeDatabase("MyConnect3");
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        if (resp == dbService::RENAME_OK)
            db_res = "RENAME_OK";
        else if(resp == dbService::RENAME_FAILED) {
            db_res = "RENAME_FAILED";
            only_this_ed = true; //the error message popup must appear only to who made the request
        }
        else if(resp == dbService::QUERY_ERROR)
            db_res = "QUERY_ERROR";
        else
            db_res = "DB_ERROR";

        //Serialize data
        json j;
        jsonUtility::to_json_rename_file(j, "RENAMEFILE_RESPONSE", db_res, new_file_name_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "OPENFILE_REQUEST") {
        std::string user_JSON;
        std::string uri_JSON;
        jsonUtility::from_json_uri(j_data, user_JSON, uri_JSON); //get json value and put into JSON variables

        //update tables on db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::openFile(user_JSON, uri_JSON);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp == dbService::OPENFILE_OK) {
            //Update session data
            shared_from_this()->set_curr_file(uri_JSON);
            shared_from_this()->set_symbols(SharedEditor::getInstance().get_file(uri_JSON, true));

            SharedEditor::getInstance().update_file(shared_from_this()->get_curr_file(),
                                                    shared_from_this()->get_symbols());

            curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened
            ed_id = shared_from_this()->get_id();

            if(shared_from_this()->get_symbols().empty()) //file is empty (it can happen that one client save an empty file)
                db_res = "OPENFILE_FILE_EMPTY";
            else
                db_res = "OPENFILE_OK";

            //Serialize data
            json j;
            jsonUtility::to_json_symbols_vector_response(j, "OPENFILE_RESPONSE", db_res,
                                                         shared_from_this()->get_symbols());
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
        jsonUtility::to_json_simple_resp(j, "OPENFILE_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "OPENWITHURI_REQUEST") {
        std::string user_JSON;
        std::string uri_JSON;
        std::string filename_JSON;
        jsonUtility::from_json_uri(j_data, user_JSON, uri_JSON); //get json value and put into JSON variables

        //update tables on db
        const char *db_res;
        dbService::DB_RESPONSE resp = dbService::openURIFile(user_JSON, uri_JSON, filename_JSON);
        QSqlDatabase::removeDatabase("MyConnect2");

        if (resp == dbService::OPENWITHURI_OK) {
            //Update session data
            shared_from_this()->set_curr_file(uri_JSON);
            shared_from_this()->set_symbols(SharedEditor::getInstance().get_file(uri_JSON, true));

            SharedEditor::getInstance().update_file(shared_from_this()->get_curr_file(),
                                                    shared_from_this()->get_symbols());

            curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened
            ed_id = shared_from_this()->get_id();

            if(shared_from_this()->get_symbols().empty()) //file is empty (it can happen that one client save an empty file)
                db_res = "OPENFILE_FILE_EMPTY";
            else
                db_res = "OPENWITHURI_OK";

            //Serialize data
            json j;
            jsonUtility::to_json_open_w_uri_resp(j, "OPENWITHURI_RESPONSE", db_res,
                                                 shared_from_this()->get_symbols(), filename_JSON);
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
        jsonUtility::to_json_simple_resp(j, "OPENWITHURI_RESPONSE", db_res);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "INVITE_URI_REQUEST") {
        std::string invited_JSON;
        std::string applicant_JSON;
        std::string uri_JSON;
        jsonUtility::from_json_inviteURI(j_data, invited_JSON, applicant_JSON, uri_JSON); //get json value and put into JSON variables

        // Get email related to that username
        const char *db_res2;
        std::string email_invited;
        dbService::DB_RESPONSE resp2 = dbService::getEmail(invited_JSON, email_invited);
        QSqlDatabase::removeDatabase("MyConnect2");

        if(resp2 == dbService::GET_EMAIL_OK) {
            // Send email
            if(email::sendEmail(email_invited, uri_JSON)) {
                const char *db_res;
                if(invited_JSON == applicant_JSON){
                    db_res = "SAME_USER";
                } else {
                    dbService::DB_RESPONSE resp = dbService::addFriend(invited_JSON, uri_JSON);
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
                jsonUtility::to_json_simple_resp(j, "INVITE_URI_RESPONSE", db_res);
                const std::string response = j.dump();
                return response;
            }
            else {
                const char *db_res = "SEND_EMAIL_FAILED";
                json j;
                jsonUtility::to_json_simple_resp(j, "INVITE_URI_RESPONSE", db_res);
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
        jsonUtility::to_json_simple_resp(j, "INVITE_URI_RESPONSE", db_res2);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "INSERTION_REQUEST") {
        Symbol symbol_JSON;
        int index_ed_JSON;
        jsonUtility::from_json_insertion_req(j_data, symbol_JSON, index_ed_JSON); //get json value and put into JSON variables
        std::cout << "symbol received: " << symbol_JSON.get_char() << "," << "ID: " << symbol_JSON.get_id().first << "," << symbol_JSON.get_id().second << std::endl;

        //process received symbol and retrieve new calculated index
        int newIndex = process(index_ed_JSON,
                               SharedEditor::getInstance().get_file(shared_from_this()->get_curr_file(),
                                                                    false), symbol_JSON);

        //Update room symbols for this file
        SharedEditor::getInstance().insert_in_file(shared_from_this()->get_curr_file(), newIndex, symbol_JSON);

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_insertion_resp(j, "INSERTION_RESPONSE", symbol_JSON, index_ed_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "REMOVAL_REQUEST") {
        std::vector<std::pair<int, int>> symbolsId;
        jsonUtility::from_json_removal_range_req(j_data, symbolsId);
        int newIndex;

        for(const std::pair<int,int>& id : symbolsId) {
            //process received symbol and retrieve new calculated index
            newIndex = get_index_by_id(
                    SharedEditor::getInstance().get_file(shared_from_this()->get_curr_file(), false), id);
            if(newIndex != -1) {
                //Update SE symbols for this file
                SharedEditor::getInstance().erase_from_file(shared_from_this()->get_curr_file(), newIndex);
            }
        }

        //Write on file if there aren't chars (to prevent 2nd client reading from file previous symbols, see 'get_file', 2nd 'if' statement)
        if(SharedEditor::getInstance().get_map().at(shared_from_this()->get_curr_file()).empty())
            fileUtility::writeFile(R"(..\Filesystem\)" + shared_from_this()->get_curr_file() + ".txt",
                                   SharedEditor::getInstance().get_map().at(shared_from_this()->get_curr_file()));

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_removal_range_resp(j, "REMOVAL_RESPONSE", symbolsId);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "FORMAT_RANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbols_id;
        int format_JSON;
        jsonUtility::from_json_format_range_req(j_data, symbols_id, format_JSON); //get json value and put into JSON variables
        int new_index;

        for(const std::pair<int, int>& id : symbols_id) {
            //process received symbol and retrieve new calculated index
            new_index = get_index_by_id(SharedEditor::getInstance().get_file(
                    shared_from_this()->get_curr_file(), false), id);
            if(new_index != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().format_in_file(shared_from_this()->get_curr_file(), new_index, format_JSON);
            }
        }

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send the message only to clients having this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_format_range_res(j, "FORMAT_RANGE_RESPONSE", symbols_id, format_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "FONTSIZE_CHANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbols_id;
        int font_sz_JSON;
        jsonUtility::from_json_fontsize_change_req(j_data, symbols_id, font_sz_JSON);
        int new_index;

        for(const std::pair<int, int>& id : symbols_id) {
            //process received symbol and retrieve new calculated index
            new_index = get_index_by_id(
                    SharedEditor::getInstance().get_file(shared_from_this()->get_curr_file(), false), id);
            if(new_index != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().ch_font_sz_in_file(shared_from_this()->get_curr_file(), new_index,
                                                               font_sz_JSON);
            }
        }

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send the message only to clients having this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_fontsize_change_resp(j, "FONTSIZE_CHANGE_RESPONSE", symbols_id, font_sz_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "FONTFAMILY_CHANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbols_id;
        std::string font_fam_JSON;
        jsonUtility::from_json_font_family_ch_req(j_data, symbols_id, font_fam_JSON);
        int new_index;

        for(const std::pair<int, int>& id : symbols_id) {
            //process received symbol and retrieve new calculated index
            new_index = get_index_by_id(
                    SharedEditor::getInstance().get_file(shared_from_this()->get_curr_file(), false), id);
            if(new_index != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().ch_font_fam_in_file(shared_from_this()->get_curr_file(), new_index,
                                                                font_fam_JSON);
            }
        }

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send the message only to clients having this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_font_family_ch_resp(j, "FONTFAMILY_CHANGE_RESPONSE", symbols_id, font_fam_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "ALIGNMENT_CHANGE_REQUEST") {
        std::vector<std::pair<int, int>> symbols_id;
        int alignment_JSON;
        jsonUtility::from_json_alignment_ch_req(j_data, symbols_id, alignment_JSON);
        int new_index = -1;

        for (const std::pair<int, int> &id : symbols_id) {
            //process received symbol and retrieve new calculated index
            new_index = get_index_by_id(
                    SharedEditor::getInstance().get_file(shared_from_this()->get_curr_file(), false), id);
            if (new_index != -1) {
                //Update room symbols for this file
                SharedEditor::getInstance().ch_alignment_in_file(shared_from_this()->get_curr_file(), new_index,
                                                                 alignment_JSON);
            }
        }

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send the message only to clients having this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_alignment_ch_resp(j, "ALIGNMENT_CHANGE_RESPONSE", symbols_id, alignment_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "CURSOR_CHANGE_REQUEST") {
        int pos_JSON;
        jsonUtility::from_json_cursor_change_req(j_data, pos_JSON);
        std::cout << "pos received: " << std::to_string(pos_JSON) << std::endl;

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        //Serialize data
        json j;
        jsonUtility::to_json_cursor_change_resp(j, "CURSOR_CHANGE_RESPONSE", shared_from_this()->get_username(),
                                                shared_from_this()->get_color(), pos_JSON);
        const std::string response = j.dump();
        return response;

    } else if (j_op == "COLLAB_COLORS_REQUEST") {
        std::string uri_JSON;
        jsonUtility::from_json_collab_colors_req(j_data, uri_JSON);
        std::cout << "uri received: " << uri_JSON << std::endl;

        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        const char *db_res;
        std::map<std::string, std::pair<std::string, bool>> mapCollabColors;
        dbService::DB_RESPONSE resp = dbService::getColors(uri_JSON, mapCollabColors);
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

    } else if (j_op == "INSERTIONRANGE_REQUEST") {
        std::vector<json> range_symbols_JSON;
        int start_index_JSON;
        jsonUtility::from_json_insertion_range_req(j_data, range_symbols_JSON, start_index_JSON);
        std::vector<Symbol> symbols = jsonUtility::from_jsons_to_syms(range_symbols_JSON);
        int new_index = start_index_JSON;

        for(const Symbol& s : symbols) {
            //process received symbol and retrieve new calculated index
            new_index = process(new_index, SharedEditor::getInstance().get_file(
                    shared_from_this()->get_curr_file(), false), s);

            //Update room symbols for this file
            SharedEditor::getInstance().insert_in_file(shared_from_this()->get_curr_file(), new_index, s);
        }

        ed_id = shared_from_this()->get_id(); //don't send this message to this editor
        curr_file = shared_from_this()->get_curr_file(); //send only the message to clients that have this curr_file opened

        //Serialize data
        json j;
        std::vector<json> symbols_JSON = jsonUtility::from_syms_to_jsons(symbols);
        jsonUtility::to_json_insertion_range_resp(j, "INSERTIONRANGE_RESPONSE", start_index_JSON, symbols_JSON);
        const std::string response = j.dump();
        return response;

    } else { //editor functions
        SharedEditor::getInstance().broadcast_deliver(msg_in); //deliver to all the participants
    }
}
