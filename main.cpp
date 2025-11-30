#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "user.h"
#include "Broker.h"
#include "marketData.h"
#include "DBManager.h"

int main()
{
    std::vector<std::string> symbols = {"BTCUSDT", "ETHUSDT", "AAPL", "IBM"};

    Broker *bkr = &Broker::getinstance();
    DBManager dbManager("trading_simulator.db");
    std::cout << "Trading Simulator Started" << std::endl;
    MarketData &md = MarketData::getInstance();
    // Start a single market data thread for all symbols
    std::thread marketThread(&MarketData::marketdataevent, &md, symbols);
    marketThread.detach();

    int query;
    std::cout << "Enter query" << std::endl;
    std::cin >> query;
    while (query != 0)
    {
        std::cout << "Enter query" << std::endl;
        std::cin >> query;
        if (query == 1)
        {
            bkr->createUser();
        }
        else if (query == 2)
        {
            bool userlogin = false;
            int userid;
            unsigned long userpassword;
            std::cin >> userid >> userpassword;
            User *user = bkr->getuserobj(userid);
            if (user && user->verifypassword(userpassword))
            {
                userlogin = true;
            }
            if (userlogin)
            {
                std::string symbol, ordermode;
                int quantity;
                double price;
                std::cin >> symbol >> quantity >> price >> ordermode;
                try
                {
                    user->placeorder(symbol, quantity, price, ordermode);
                }
                catch (...)
                {
                    std::cout << "Order placement failed" << std::endl;
                }
            }
        }
        else if (query == 3)
        {
            bool userlogin = false;
            int userid;
            unsigned long userpassword;
            std::cin >> userid >> userpassword;
            User *user = bkr->getuserobj(userid);
            if (user && user->verifypassword(userpassword))
            {
                userlogin = true;
            }
            if (userlogin)
            {
                for (auto it : user->portfolio)
                {
                    std::cout << it.first << " " << it.second << std::endl;
                }
                std::cout << "balance is: " << user->getBalance() << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid Input" << std::endl;
        }
        std::cin >> query;
    }
    delete bkr;
    return 0;
}