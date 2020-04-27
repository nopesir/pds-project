//
// Created by gheb on 24/04/20.
//

#include "Session.h"
#include "SharedEditor.h"

Session::Session(boost::asio::ip::tcp::socket socket, SharedEditor &se, int id)
        : Client(id), _socket(std::move(socket)), _se(se) {
    session_start();
}

void Session::session_start() {
    std::shared_ptr<Client> sp(this);
    _se.join(sp);
}