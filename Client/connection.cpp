#include "connection.h"

Connection::Connection(std::string adress, int port) : socket(ioc)
{
    try {
        tcp::resolver resolver(ioc);
        auto endpoints = resolver.resolve(adress, std::to_string(port));
        net::connect(socket, endpoints);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

Connection::~Connection()
{
    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

tcp::socket &Connection::getSocket()
{
    return socket;
}

std::string Connection::toJson(std::unordered_map<std::string, std::string> &data)
{
    boost::property_tree::ptree root;
    for (const auto& [header, value] : data) {
        root.put(header, value);
    }
    std::stringstream str;
    boost::property_tree::write_json(str, root);
    return str.str();
}

void Connection::sendToServer(std::unordered_map<std::string, std::string> &data)
{
    std::string msg = toJson(data);
    boost::system::error_code ec;
    socket.write_some(net::buffer(msg.data(), msg.size()), ec);

    emit readyRead();
}

boost::property_tree::ptree Connection::receiveFromServer()
{
    boost::property_tree::ptree root;
    while (true) {
        size_t bytes = socket.available();
        if (bytes == 0) continue;
        std::vector<char> buff(bytes);
        boost::system::error_code ec;

        socket.read_some(net::buffer(buff.data(), buff.size()), ec);

        std::stringstream json;
        for (auto ch : buff) {
            json << ch;
        }
        boost::property_tree::read_json(json, root);
        break;
    }

    return root;
}
