#ifndef USER_H
#define USER_H
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class User {
private:
    short int  _userId;
    unsigned long _password;
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
    bool setpassword(unsigned long password);
    bool verifypassword(unsigned long password);
    void placeorder(const string& symbol, int quantity, double price, const string& ordermode);
    void updateUser(unsigned long balance, unsigned long pnlOfDay, unsigned long totalPnl, unsigned long margin);
    unsigned long getPnlOfDay() const;
    unsigned long getTotalPnl() const;
    unsigned long getPassword() const;
    void addToPortfoliofromDB(const string& portfolioStr);
};

#endif // USER_H
