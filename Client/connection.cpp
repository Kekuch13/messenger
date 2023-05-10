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

std::string Connection::toJson(std::unordered_map<std::string, std::string> &data) // перевод запроса от клиента в json
{
    boost::property_tree::ptree root;
    for (const auto& [header, value] : data) {
        root.put(header, value);
    }
    std::stringstream str;
    boost::property_tree::write_json(str, root);
    return str.str();
}

void Connection::sendToServer(std::unordered_map<std::string, std::string> &data) // отправка запросов на сервер
{
    std::string msg = toJson(data);
    boost::system::error_code ec;
    socket.write_some(net::buffer(msg.data(), msg.size()), ec);

    emit readyRead();
}

boost::property_tree::ptree Connection::receiveFromServer() // прием ответов от сервера
{
    socket.wait(socket.wait_read);
    size_t bytes = socket.available();
    std::vector<char> buff(bytes);
    boost::system::error_code ec;

    socket.read_some(net::buffer(buff.data(), buff.size()), ec);

    // перевод поступившего сообщения из json в boost::property_tree для дельнейшей обработки
    std::stringstream json;
    for (auto ch : buff) {
        json << ch;
    }
    boost::property_tree::ptree root;
    boost::property_tree::read_json(json, root);

    return root;
}
