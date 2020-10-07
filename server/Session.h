//
// Created by gheb on 24/04/20.
//

#ifndef PROJECT_SESSION_H
#define PROJECT_SESSION_H


#include <boost/asio/ip/tcp.hpp>
#include <deque>
#include<boost/filesystem.hpp>
#include "SharedEditor.h"
#include "Message.h"
#include "jsonUtility.h"
#include "dbService.h"

class Session: public Client, public std::enable_shared_from_this<Session>{
private:
    boost::asio::ip::tcp::socket _socket;
    Message msg_in;
    std::deque<Message> msgs_out;
    std::string body;

    void read_header();
    void read_body();
    void write();
    std::string process_reqs(const std::string& j_op, const json& j_data, int& ed_id, std::string& curr_file, bool& only_this_ed);
    void send_msg(const std::string& response);
    void send_msg_all(const std::string& response, const int& ed_id, const std::string& curr_file, bool include_this_ed= false); //send msg to all the clients except client with id 'ed_id' having the curr_file opened
public:
    explicit Session(boost::asio::ip::tcp::socket socket);
    void start_session(int ed_id);
    void deliver(const Message& msg);

};


#endif //PROJECT_SESSION_H
