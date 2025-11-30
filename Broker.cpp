#include "Broker.h"
#include "user.h"
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <unordered_map>

Broker::Broker() : _totalusers(0)
{
    loadDataFromDB();
}

Broker &Broker::getinstance()
{
    static Broker instance;
    return instance;
}

Broker::~Broker()
{
    updateDataToDB();
    for (auto &p : Broker::_listofuser)
    {
        delete p.second; // delete the object
    }
    _listofuser.clear();
}

void Broker::loadDataFromDB()
{
    // load all users from db and populate _listofuser
    try
    {
        sqlite3 *db;
        sqlite3_stmt *stmt;
        int rc = sqlite3_open("trading_simulator.db", &db);
        if (rc)
        {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "Opened database successfully" << std::endl;
        }
        // load all data from db
        const std::string userdataquery = "SELECT * FROM users;";
        sqlite3_prepare_v2(db, userdataquery.c_str(), -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            User *user = new User(sqlite3_column_int(stmt, 0));
            const unsigned char *text = sqlite3_column_text(stmt, 7);
            if (text != nullptr)
            {
                user->addToPortfoliofromDB(std::string(reinterpret_cast<const char *>(text)));
            }
            user->addBalance(sqlite3_column_int(stmt, 3));
            user->setPnlOfDay(sqlite3_column_int(stmt, 4));
            user->setTotalPnl(sqlite3_column_int(stmt, 5));
            _listofuser[sqlite3_column_int(stmt, 0)] = user;
        }
        const std::string userpasswordquery = "SELECT * FROM user_passwords;";
        sqlite3_prepare_v2(db, userpasswordquery.c_str(), -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int userid = sqlite3_column_int(stmt, 0);
            std::string password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            if (_listofuser.find(userid) != _listofuser.end())
            {
                _listofuser[userid]->setpassword(std::stoul(password));
            }
        }
        sqlite3_close(db);
    }
    catch (...)
    {
        std::cout << "Error in connecting to database" << std::endl;
    }
}

void Broker::updateDataToDB()
{
    // update all users data to db
    try
    {
        sqlite3 *db;
        sqlite3_stmt *stmt;
        int rc = sqlite3_open("trading_simulator.db", &db);
        if (rc)
        {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "Opening db for updating data" << std::endl;
        }
        for (auto &it : _listofuser)
        {
            User *user = it.second;
            std::string insertuserdataquery = "INSERT OR REPLACE INTO users (userId, symbol, quantity, balance, pnlOfDay, totalPnl) VALUES (?,?,?,?,?,?);";
            sqlite3_prepare_v2(db, insertuserdataquery.c_str(), -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, user->getUserId());
            for (auto &stock : user->portfolio)
            {
                sqlite3_bind_text(stmt, 2, stock.first.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 3, stock.second);
            }
            sqlite3_bind_int(stmt, 4, user->getBalance());
            sqlite3_bind_int(stmt, 5, user->getPnlOfDay());
            sqlite3_bind_int(stmt, 6, user->getTotalPnl());
            sqlite3_step(stmt);
            std::string insertuserpasswordquery = "INSERT OR REPLACE INTO user_passwords (userId, password) VALUES (?,?);";
            sqlite3_prepare_v2(db, insertuserpasswordquery.c_str(), -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, user->getUserId());
            // If you want to store password as string, convert it
            sqlite3_bind_text(stmt, 2, std::to_string(user->getPassword()).c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
        }
        sqlite3_close(db);
    }
    catch (...)
    {
        std::cout << "Error in updating database" << std::endl;
    }
}

void Broker::createUser()
{
    _totalusers++;
    User *user = new User(_totalusers);
    _listofuser[_totalusers] = user;
}

User *Broker::getuserobj(int userid)
{
    if (_listofuser.find(userid) != _listofuser.end())
        return _listofuser[userid];
    return nullptr;
}