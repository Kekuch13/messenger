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

tcp::socket& Connection::getSocket() {
    return socket;
}

std::string Connection::toJson(std::unordered_map<std::string, std::string>& data) {
    boost::property_tree::ptree root;
    for(const auto& [header, value] : data) {
        root.put(header, value);
    }
    std::stringstream str;
    boost::property_tree::write_json(str, root);
    return str.str();
}


void Connection::sendToServer(std::unordered_map<std::string, std::string>& data) {
    std::string msg = toJson(data);
    socket.send(net::buffer(msg));

    emit readyRead();
}

boost::property_tree::ptree Connection::receiveFromServer() {
    std::array<char, 1024> buff{};
    boost::system::error_code ec;

    size_t len = socket.read_some(net::buffer(buff), ec);

    std::stringstream json;
    json.write(buff.data(), len);
    boost::property_tree::ptree root;
    boost::property_tree::read_json(json, root);

    return root;
}
