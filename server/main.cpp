//
// Created by gheb on 05/04/20.
//

#include "Server.h"
#include <boost/asio.hpp>

int main(){
    //srand(time(nullptr));
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::atoi("63505"));
    Server s(io_context, endpoint);
    io_context.run();
}
