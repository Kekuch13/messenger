#pragma once

#include <iostream>
#include <pqxx/pqxx>
#include <string>

class DatabaseManager {
private:
    std::string host = "localhost";
    std::string port = "5432";
    std::string dbname = "messenger";
    std::string user = "postgres";
    std::string password = "Happy2022";
    std::string connectionString() const;

    pqxx::connection conn;
    void prepare_statements();
public:
    DatabaseManager();

    pqxx::connection &GetConn();
};