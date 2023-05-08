#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>

namespace net = boost::asio;
using boost::asio::ip::tcp;

class Connection
{
private:
    net::io_context ioc{1};
    tcp::socket socket;
    tcp::resolver::results_type endpoints;
public:
    Connection(std::string adress, int port);

    void sendToServer(std::string msg);
    std::string receiveFromServer();
};

#endif // CONNECTION_H
