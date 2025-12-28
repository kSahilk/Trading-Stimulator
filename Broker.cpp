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
    std::cout<<"db done"<<std::endl;
std::cout<<lengthoflistofUser();
    for (auto &p : Broker::_listofuser)
    {
        if(p.second){
            delete p.second; // delete the object
        }
    }
    if(lengthoflistofUser()!=0){
        _listofuser.clear();
    }
}

void Broker::loadDataFromDB()
{
    // load all users from db and populate _listofuser
    try
    {
        sqlite3 *db;
        sqlite3_stmt *stmt;
        if (sqlite3_open("trading_simulator.db", &db))
        {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        } 
        else
        {
            std::cout << "Opened database successfully" << std::endl;
        }
        const std::string userDataQuery = "SELECT * FROM users;";
        sqlite3_prepare_v2(db, userDataQuery.c_str(), -1, &stmt, NULL);
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
        const std::string userPasswordQuery = "SELECT * FROM user_passwords;";
        sqlite3_prepare_v2(db, userPasswordQuery.c_str(), -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int userid = sqlite3_column_int(stmt, 0);
            std::string password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            if (_listofuser.find(userid) != _listofuser.end())
            {
                _listofuser[userid]->setpassword(password);
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
    sqlite3 *db;
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_open("trading_simulator.db", &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    for (auto &it : _listofuser)
    {
        User *user = it.second;
        std::string assets;
        std::string assets_qty;
        bool first = true;
        for (auto &stock : user->portfolio)
        {
            if (!first)
            {
                assets.push_back(',');
                assets_qty.push_back(',');
            }
            assets += stock.first;
            assets_qty += std::to_string(stock.second);
            first = false;
        }
        std::string query = "INSERT OR REPLACE INTO user (ID, Assets, Assets_Quantity, Balance, PnlOfDay, TotalPnl, Margin) VALUES (?,?,?,?,?,?,?);";
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
            if (stmt) sqlite3_finalize(stmt);
            stmt = nullptr;
            continue;
        }
        sqlite3_bind_int(stmt, 1, user->getUserId());
        sqlite3_bind_text(stmt, 2, assets.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, assets_qty.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, user->getBalance());
        sqlite3_bind_int(stmt, 5, user->getPnlOfDay());
        sqlite3_bind_int(stmt, 6, user->getTotalPnl());
        sqlite3_bind_int(stmt, 7, 0);
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "Insert failed for user " << user->getUserId() << ": " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    sqlite3_close(db);
    std::cout << "db is updated" << std::endl;
}


void Broker::createUser()
{
    _totalusers++;
    string password;
    std::cout << "Enter password for new user: ";
    cin >> password;
    cout<<"Total users inside create user "<<_totalusers<<endl;
    User *user = new User(_totalusers);
    user->setpassword(password);
    _listofuser[_totalusers] = user;
    std::cout << "User Created Succssfully" << std::endl;
}

User *Broker::getuserobj(int userid)
{
    if (_listofuser.find(userid) != _listofuser.end())
        return _listofuser[userid];
    return nullptr;
}

bool Broker::checkUserId(int userid)
{
    return _listofuser.find(userid) != _listofuser.end();
}

unsigned long Broker::lengthoflistofUser(){
    std::cout<<"length called inside"<<std::endl;
return _listofuser.size();
}