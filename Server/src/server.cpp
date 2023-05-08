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
        std::shared_ptr<tcp::socket> newConnection = std::make_shared<tcp::socket>(ioc);
        acceptor.accept(*newConnection.get());
//        std::cout << newConnection->is_open() << " ======== " << acceptor.is_open() << std::endl;
        std::cout << "Client accepted!\n";
        std::thread([this, newConnection] { return this->session(newConnection); }).detach();
//        std::cout << newConnection->is_open() << " ******** " << acceptor.is_open() << std::endl;
    }
}

void Server::session(std::shared_ptr<tcp::socket> socket) {
    try {
        std::array<char, 1024> buff{};
        boost::system::error_code ec;
        while (true) {
//            std::cout << socket->is_open() << " ++++++++ " << acceptor.is_open() << std::endl;
            size_t len = socket->read_some(net::buffer(buff), ec);
            if (ec == net::error::eof) {
                break;
            } else if (ec) {
                throw boost::system::system_error(ec);
            }

            std::cout.write(buff.data(), len);

            boost::property_tree::ptree root = requestToPtree(buff, len);
            std::string requestName = root.get<std::string>("requestName");

            if (requestName == "authorization") {
                pqxx::work worker(dbManager.GetConn());
                auto result =
                    worker.exec_prepared("authorization",
                                         root.get<std::string>("login"),
                                         root.get<std::string>("password"));
                if (result.size() == 1) {
                    clients[root.get<std::string>("login")] = socket;
//                    std::cout << "success\n";
                    sendResponse(socket, "{\"responseName\":\"authorization\",\n\"status\":\"success\"}");
                } else {
//                    std::cout << "fail\n";
                    sendResponse(socket, "{\"responseName\":\"authorization\",\n\"status\":\"fail\"}");
                }
            } else {
                requestHandler(root);
            }
        }
        socket->shutdown(tcp::socket::shutdown_send, ec);
        if (ec) {
            throw boost::system::system_error(ec);
        } else {
            std::cout << "Client disconnected\n";
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

boost::property_tree::ptree Server::requestToPtree(std::array<char, 1024> &buff, size_t len) {
    std::stringstream json;
    json.write(buff.data(), len);
    boost::property_tree::ptree root;
    boost::property_tree::read_json(json, root);
    return root;
}

void Server::sendResponse(std::shared_ptr<tcp::socket> socket, std::string msg) {
    try {
        boost::system::error_code ec;
        socket->write_some(net::buffer(msg), ec);
        if (ec) {
            throw boost::system::system_error(ec);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}

void Server::requestHandler(boost::property_tree::ptree &root) {
    std::string requestName = root.get<std::string>("requestName");

    if (requestName == "getDialogs") {
        pqxx::work worker(dbManager.GetConn());
        auto result = worker.exec_prepared("allDialogs");

        boost::property_tree::ptree ptree, children;
        ptree.put("responseName", "dialogs");
        for (pqxx::result::size_type i = 0; i < result.size(); ++i) {
            boost::property_tree::ptree child;
            child.put("", result[i][0]);
            children.push_back(std::make_pair("", child));
        }
        ptree.add_child("dialogs", children);

        std::stringstream data;
        boost::property_tree::write_json(data, ptree);
        sendResponse(clients[root.get<std::string>("username")], data.str());
    } else if (false) {

    }
}
