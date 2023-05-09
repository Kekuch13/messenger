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
    conn.prepare("registration", "INSERT INTO users VALUES ($1, $2)");
    conn.prepare("findUser", "SELECT * from users WHERE username=$1");
    conn.prepare("findDialog", "SELECT id\n"
                               "FROM dialogs\n"
                               "WHERE (username_1=$1 AND username_2=$2) OR (username_2=$1 AND username_1=$2)");
    conn.prepare("authorization", "SELECT * FROM users WHERE username=$1 AND password=$2");
    conn.prepare("allDialogs",
                 "SELECT id, username_1 AS username FROM dialogs WHERE username_2=$1 UNION SELECT id, username_2 FROM dialogs WHERE username_1=$1");
    conn.prepare("newDialogs",
                 "SELECT username FROM users WHERE username NOT IN (SELECT username_1 FROM dialogs WHERE username_2=$1 UNION SELECT username_2 FROM dialogs WHERE username_1=$1)");
    conn.prepare("createDialog", "INSERT INTO dialogs(username_1, username_2) VALUES ($1, $2) RETURNING id");
    conn.prepare("addMessage", "INSERT INTO messages(dialog_id, author, text) VALUES ($1, $2, $3)");
    conn.prepare("findRecipient", "SELECT username_1 AS recipient\n"
                                  "FROM dialogs INNER JOIN messages m on dialogs.id = m.dialog_id\n"
                                  "WHERE dialogs.id = $1 AND author = $2 AND username_2 = $2\n"
                                  "UNION\n"
                                  "SELECT username_2 AS recipient\n"
                                  "FROM dialogs INNER JOIN messages m on dialogs.id = m.dialog_id\n"
                                  "WHERE dialogs.id = $1 AND author = $2 AND username_1 = $2");
    conn.prepare("dialogMessages", "SELECT author, text FROM messages WHERE dialog_id=$1 ORDER BY time ASC");
}

pqxx::connection &DatabaseManager::GetConn() {
    return conn;
}