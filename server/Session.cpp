//
// Created by gheb on 24/04/20.
//

#include "Session.h"

Session::Session(boost::asio::ip::tcp::socket socket, int id): _socket(std::move(socket)),
_user(User(id, "temporary_name")){
    session_start();
}

void Session::session_start() {
    shared_from_this()->_user.setFile("");
}
