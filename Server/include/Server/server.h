#pragma once

#include <boost/asio.hpp>
//#include "connection.h"
#include <functional>
#include <string>

using net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Server {
private:
    int port;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::acceptor acceptor;

//    std::vector<TCPConnection::pointer> _connections {};
public:
    Server(int port);

    int Run();
    void AcceptClients();
};