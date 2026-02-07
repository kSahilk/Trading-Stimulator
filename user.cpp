#include "user.h"
#include "marketData.h"
#include <algorithm>
#include <cstdlib>
// #include<iostea

User::User(int id) : _userId(id), _password(""), _balance(0), _pnlOfDay(0), _totalPnl(0), _margin(0) {
    std::cout<<"construcotr done"<<std::endl;
}


short int User::getUserId() const { return _userId; }
unsigned long User::getBalance() const { return _balance; }
unsigned long User::getPnlOfDay() const { return _pnlOfDay; }
unsigned long User::getTotalPnl() const { return _totalPnl; }
string User::getPassword() const { return _password; }

void User::addBalance(unsigned long balance) { _balance += balance; }
void User::setPnlOfDay(unsigned long pnl) { _pnlOfDay = pnl; }
void User::setTotalPnl(unsigned long pnl) { _totalPnl = pnl; }

void User::setpassword(std::string password) {
    _password = password;
    std::cout<<"Password setted";
}

bool User::verifypassword(string password) {
    return _password == password;
}

void User::addToPortfolio(const string& symbol, int quantity) {
    for (auto& stock : portfolio) {
        if (stock.first == symbol) {
            stock.second += quantity;
            return;
        }
    }
    portfolio.push_back({symbol, quantity});
}

void User::updateUser(double price, int quantity, const string& symbol) {
    addBalance(-(price * quantity));
    addToPortfolio(symbol, quantity);
    setPnlOfDay(_pnlOfDay + (price * quantity));
    setTotalPnl(_totalPnl + (price * quantity));
}

void User::addToPortfoliofromDB(const string &text) {
    int n = text.size();
    for (int i = 0; i < n; ) {
        string symbol = "";
        string qtyStr = "";

        while (i < n && text[i] != ',') symbol += text[i++];
        i++;

        while (i < n && text[i] != ',') qtyStr += text[i++];
        i++;

        portfolio.push_back({symbol, stoi(qtyStr)});
    }
}
