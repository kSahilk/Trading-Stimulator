#ifndef BROKER_H
#define BROKER_H
#include <unordered_map>
#include <string>
#include "user.h"

class Broker {
private:
    int _totalusers;
    Broker();
    Broker(const Broker&) = delete;
    Broker& operator=(const Broker&) = delete;
    std::unordered_map<int, User*> _listofuser;

    public:
    static Broker& getinstance();
    ~Broker();
    void createUser();
    User* getuserobj(int userid);
    void loadDataFromDB();
    void updateDataToDB();
    bool checkUserId(int userid) ;
    void lengthoflistofUser();
};

#endif // BROKER_H
