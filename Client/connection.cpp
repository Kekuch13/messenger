#include "connection.h"

Connection::Connection(std::string adress, int port) : socket(ioc) {
    try {
        tcp::resolver resolver(ioc);
        auto endpoints = resolver.resolve(adress, std::to_string(port));
        net::connect(socket, endpoints);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Connection::sendToServer(std::string msg) {
    socket.send(net::buffer(msg));
}

