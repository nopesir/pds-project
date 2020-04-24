//
// Created by gheb on 05/04/20.
//

#include "Server.h"
#include "Session.h"

#include <iostream>
#include <memory>
#include <utility>
//#include "session.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint)
        : acceptor(io_context, endpoint), editor_counter(0) {
    accept_connection();
}

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "InfiniteRecursion"
void Server::accept_connection()
{
    std::cout << "Waiting for clients..." << std::endl;
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (ec) {
            std::cout << "Error before handle_accept: " << ec.message() << std::endl;
            return; //TODO Server never return. Maybe is better to change this behaviour
        }
       // std::make_shared<session>(std::move(socket), room_)->session_start(++editorCounter_); //start new socket for the new client
       std::make_shared<Session>(std::move(socket), ++editor_counter)->session_start();
       accept_connection(); //server socket continue accepting new connections
    });
}
//#pragma clang diagnostic pop

