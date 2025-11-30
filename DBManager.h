#pragma once
#include <sqlite3.h>
#include <string>
#include <iostream>

class DBManager {
private:
    sqlite3* DB;

    void createTables();

public:
    DBManager(const std::string& dbName);
    ~DBManager();

    sqlite3* getDB();
};
