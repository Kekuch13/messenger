#include "Server/server.h"
#include <iostream>

Server::Server(int port) : acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {}

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
        std::shared_ptr<tcp::socket> newConnection = std::make_shared<tcp::socket>(ioc) ;
        acceptor.accept(*newConnection.get());
//        std::cout << newConnection->is_open() << " ======== " << acceptor.is_open() << std::endl;
        std::cout << "Client accepted!\n";
        std::thread([this, newConnection] { return this->session(newConnection); }).detach();
//        std::cout << newConnection->is_open() << " ******** " << acceptor.is_open() << std::endl;
    }
}

void Server::session(std::shared_ptr<tcp::socket> socket) {
    try {
        std::array<char, 1024> buf {};
        boost::system::error_code ec;
        while (true) {
//            std::cout << socket->is_open() << " ++++++++ " << acceptor.is_open() << std::endl;
            size_t len = socket->read_some(boost::asio::buffer(buf), ec);

            if (ec == net::error::eof) {
                // Clean connection cut off
                break;
            } else if (ec) {
                throw boost::system::system_error(ec);
            }

            std::cout.write(buf.data(), len);
//            requestHandler(buff);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Server::sendResponse(tcp::socket sock, std::string msg) {

}

void Server::requestHandler(net::streambuf& buff) {
    std::stringstream m;
    m << std::istream{&buff}.rdbuf();
    std::cout << m.str() << "\n";
}
