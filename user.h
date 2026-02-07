#ifndef USER_H
#define USER_H
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
using namespace std;

class User {
private:
    short int  _userId;
    string _password;
    unsigned long _balance;
    unsigned long _pnlOfDay;
    unsigned long _totalPnl;
    unsigned long _margin;
public:
    vector<pair<string,int>> portfolio;
    unordered_map<int,string> userPasswordMap;
    User(int id);
    short int getUserId() const;
    unsigned long getBalance() const;
    void addBalance(unsigned long balance);
    void setPnlOfDay(unsigned long pnl);
    void setTotalPnl(unsigned long pnl);
    void addToPortfolio(const string& symbol, int quantity);
    void  setpassword(string password);
    bool verifypassword(string password);
    void updateUser(double price, int quantity, const string& symbol) ;
    unsigned long getPnlOfDay() const;
    unsigned long getTotalPnl() const;
    string getPassword() const;
    void addToPortfoliofromDB(const string& portfolioStr);
};

#endif // USER_H
