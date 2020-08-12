//
// Created by gheb on 24/04/20.
//

#ifndef PROJECT_SESSION_H
#define PROJECT_SESSION_H


#include <boost/asio/ip/tcp.hpp>
#include <deque>

#include "SharedEditor.h"
#include "Message.h"
#include "jsonUtility.h"
#include "dbService.h"

class Session: public Client, public std::enable_shared_from_this<Session>{
private:
    boost::asio::ip::tcp::socket _socket;
    Message read_msg_;
    std::deque<Message> write_msgs_;
    std::string fullBody;

    dbService dbService_;

    void do_read_header();
    void do_read_body();
    void do_write(); //for the editor
    std::string handleRequests(const std::string& opJSON, const json& jdata_in, int& edId, std::string& curFile, bool& onlyToThisEditor);
    void sendMsg(const std::string& response);
    void sendMsgAll(const std::string& response, const int& edId, const std::string& curFile, bool includeThisEditor=false); //send msg to all the clients except client with id 'edId' having the curFile opened
public:
    explicit Session(boost::asio::ip::tcp::socket socket);
    void session_start(int editorId);
    void deliver(const Message& msg);

};


#endif //PROJECT_SESSION_H
