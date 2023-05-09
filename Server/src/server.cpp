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
        boost::system::error_code ec;
        while (true) {
//            std::cout << socket->is_open() << " ++++++++ " << acceptor.is_open() << std::endl;

            size_t bytes = socket->available();
            if (bytes == 0) continue;
            std::cout << "Bytes: " << bytes << std::endl;
            std::vector<char> buff(bytes);
//            std::array<char, 1024> buff{};
//            size_t len = socket->read_some(net::buffer(buff.data(), buff.size()), ec);
            socket->read_some(net::buffer(buff.data(), buff.size()), ec);
            if (ec == net::error::eof) {
                break;
            } else if (ec) {
                throw boost::system::system_error(ec);
            }

//            std::cout.write(buff.data(), len);

//            boost::property_tree::ptree root = requestToPtree(buff, len);

            boost::property_tree::ptree root;
            std::stringstream json;

            for (auto ch : buff) {
                std::cout << ch;
                json << ch;
            }
            boost::property_tree::read_json(json, root);

            auto requestName = root.get<std::string>("requestName");

            if (requestName == "authorization") {
                pqxx::work worker(dbManager.GetConn());
                auto result =
                    worker.exec_prepared("authorization",
                                         root.get<std::string>("login"),
                                         root.get<std::string>("password"));
                worker.commit();
                if (result.size() == 1) {
                    clients[root.get<std::string>("login")] = socket;
                    sendResponse(socket, "{\"responseName\":\"authorization\",\n\"status\":\"success\"}");
                } else {
                    sendResponse(socket, "{\"responseName\":\"authorization\",\n\"status\":\"fail\"}");
                }
            } else if(requestName == "registration") {
                pqxx::work worker(dbManager.GetConn());
                auto result = worker.exec_prepared("findUser", root.get<std::string>("login"));
                if (result.size() == 1) {
                    sendResponse(socket, "{\"responseName\":\"registration\",\n\"status\":\"fail\"}");
                } else {
                    worker.exec_prepared("registration", root.get<std::string>("login"), root.get<std::string>("password"));
                    sendResponse(socket, "{\"responseName\":\"registration\",\n\"status\":\"success\"}");
                }
                worker.commit();
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
        std::cout << "-------\n" << msg << "-------\n";
        boost::system::error_code ec;
        socket->write_some(net::buffer(msg.data(), msg.size()), ec);
        if (ec) {
            throw boost::system::system_error(ec);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}

void Server::requestHandler(boost::property_tree::ptree &root) {
    try {
        std::string requestName = root.get<std::string>("requestName");
        std::string username = root.get<std::string>("username");

        if (requestName == "getDialogs") {
            pqxx::work worker(dbManager.GetConn());
            auto result = worker.exec_prepared("allDialogs", username);
            worker.commit();

            boost::property_tree::ptree ptree, children;
            ptree.put("responseName", "dialogs");
            for (pqxx::result::size_type i = 0; i < result.size(); ++i) {
                boost::property_tree::ptree child;
                for (pqxx::row::size_type j = 0; j < result[0].size(); ++j) {
                    if (to_string(result[i][j]) != root.get<std::string>("username")) {
                        child.put(result.column_name(j), result[i][j]);
                    }
                }
                if (!child.empty()) children.push_back(std::make_pair("", child));
            }
            ptree.add_child("dialogs", children);

            std::stringstream data;
            boost::property_tree::write_json(data, ptree);
            sendResponse(clients[username], data.str());
        } else if (requestName == "sendNewMessage") {
            auto text = root.get<std::string>("text");
            auto dialog_id = root.get<int>("dialog_id");
            pqxx::work worker(dbManager.GetConn());
            worker.exec_prepared("addMessage", dialog_id, username, text);
            worker.commit();

            pqxx::result res = worker.exec_prepared("findRecipient", dialog_id, username);
            worker.commit();
            auto recipient = res[0]["recipient"].as<std::string>();
            if (clients.contains(recipient)) {
                boost::property_tree::ptree ptree;
                ptree.put("responseName", "addNewMessage");
                ptree.put("author", username);
                ptree.put("dialog_id", dialog_id);
                ptree.put("text", text);

                std::stringstream data;
                boost::property_tree::write_json(data, ptree);
                sendResponse(clients[recipient], data.str());
            }
            successResponse(clients[username]);
        }
    } catch (std::exception &e) {
        std::cerr << e.what();
    }
}

void Server::successResponse(std::shared_ptr<tcp::socket> socket) {
    sendResponse(socket, "{\"responseName\":\"success\"}");
}
