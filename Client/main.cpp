#include "mainwidget.h"
#include <fstream>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    std::ifstream config("config.json");
    boost::property_tree::ptree data;
    boost::property_tree::read_json(config, data);
    auto connPtr = std::make_shared<Connection>(data.get<std::string>("address"), data.get<int>("port"));

    MainWidget w(nullptr, connPtr);
    w.show();

    return a.exec();
}
