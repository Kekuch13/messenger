#include "Server/server.h"
#include <iostream>

Server::Server(int port) :
    port(port),
    acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {}

int Server::Run() {
    try {
        AcceptClients();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void Server::AcceptClients() {
    while (true) {
        auto newConnection = tcp::socket(ioc);

        acceptor.accept(newConnection);
        std::cout << "Client accepted!\n";
        std::thread([this, &newConnection] { return this->session(newConnection); }).detach();
    }
}

void Server::session(tcp::socket &socket) {
    try {
        net::streambuf buffer;
        while (true) {
            socket.receive(buffer);
            requestHandler(buffer);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Server::sendResponse(tcp::socket sock, std::string msg) {

}

void Server::requestHandler(net::streambuf &buff) {
    std::string s{net::buffers_begin(buff.data()), net::buffers_end(buff.data())};
    std::cout << s << "\n";

}
