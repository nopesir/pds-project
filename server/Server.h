//
// Created by gheb on 05/04/20.
//

#ifndef PDS_PROJECT_SERVER_H
#define PDS_PROJECT_SERVER_H


//#include <room.h>

#include <boost/asio/ip/tcp.hpp>
#include "SharedEditor.h"
using boost::asio::ip::tcp;
class Server {

private:
    boost::asio::ip::tcp::acceptor acceptor;
    int cnt;
    void accept();

public:
    Server(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint);
};



#endif //PDS_PROJECT_SERVER_H
