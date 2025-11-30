#include "DBManager.h"

DBManager::DBManager(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &DB)) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(DB) << std::endl;
        DB = nullptr;
    }
    createTables();
    std::cout << "Database initialized successfully!" << std::endl;
}

DBManager::~DBManager() {
    if (DB) sqlite3_close(DB);
}

void DBManager::createTables() {
    if (!DB) return;

    std::string userTable = "CREATE TABLE IF NOT EXISTS user ("
                            "_userId SMALLINT PRIMARY KEY, "
                            "_password UNSIGNED BIGINT, "
                            "_balance UNSIGNED BIGINT DEFAULT 0, "
                            "_pnlOfDay UNSIGNED BIGINT DEFAULT 0, "
                            "_totalPnl UNSIGNED BIGINT DEFAULT 0, "
                            "_margin UNSIGNED BIGINT DEFAULT 0, "
                            "portfolio TEXT, "
                            "userPasswordMap TEXT);";

    std::string tradesTable = "CREATE TABLE IF NOT EXISTS trades ("
                              "TradeID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "_userId SMALLINT, "
                              "symbol TEXT, "
                              "price REAL, "
                              "quantity INT, "
                              "time TEXT, " 
                              "ordermode TEXT);";

    char* errMsg;
    if (sqlite3_exec(DB, userTable.c_str(), nullptr, 0, &errMsg) != SQLITE_OK)
        std::cerr << "Error creating user table: " << errMsg << std::endl;

    if (sqlite3_exec(DB, tradesTable.c_str(), nullptr, 0, &errMsg) != SQLITE_OK)
        std::cerr << "Error creating trades table: " << errMsg << std::endl;

    std::cout << "Tables created successfully!" << std::endl;
}

sqlite3* DBManager::getDB() {
    return DB;
}
