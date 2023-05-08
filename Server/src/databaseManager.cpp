#include "Server/databaseManager.h"

DatabaseManager::DatabaseManager() : conn(connectionString().c_str()) {
    if (!conn.is_open()) {
        std::cerr << "Can't open database\n";
    } else {
        prepare_statements();
    }
}

std::string DatabaseManager::connectionString() const {
    std::string connectionString =
        "host=" + host + " port=" + port + " dbname=" + dbname + " user=" + user + " password=" + password;
    return connectionString;
}

void DatabaseManager::prepare_statements() {
    conn.prepare("");
}

pqxx::connection &DatabaseManager::GetConn() {
    return conn;
}