//
// Created by gheb on 05/04/20.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"
#pragma ide diagnostic ignored "cert-msc32-c"
#include "Server.h"
#include <boost/asio.hpp>
#include <QtSql/QSqlDatabase>
#include "dbService.h"
#include <iostream>
#include <memory>



void cleanDBFlags() {
    dbService::tryCleanAll();
    QSqlDatabase::removeDatabase("MyConnect2");
}

int main(){

    try {
        cleanDBFlags();
        srand(time(nullptr));
        boost::asio::io_context io_context;
        //create endpoint with protocol and port number
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::atoi("63505"));
        //pass it to the Server class
        Server s(io_context, endpoint);
        io_context.run();

    }catch (std::exception& e)
    {
        cleanDBFlags();
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
