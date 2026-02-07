#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "user.h"
#include "Broker.h"
#include "marketData.h"
#include "DBManager.h"
#include "autoFreezeOrder.h"
#include "orderManager.h"

int main()
{
    DBManager dbManager("trading_stimulator.db");
    Broker *broker = &Broker::getinstance();
    std::cout << "Trading Stimulator Started" << std::endl;
    MarketData &MarketInstance = MarketData::getInstance();
    // Start a single market data thread for all symbols
    std::thread marketThread(&MarketData::marketdataevent, &MarketInstance);

    orderManager om;
    int query;
    std::cout << "Enter query" << std::endl;
    std::cout << "  1. Create New User" << std::endl;
    std::cout << "  2. Place Order (Manual/Strategy)" << std::endl;
    std::cout << "  3. View Portfolio" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cin >> query;
    while (query!=0)
    {
        if (query == 1)
        {
            broker->createUser();
        }
        else if (query == 2)
        {
            std::cout<<broker->lengthoflistofUser();
            int userid;
            string userpassword;
            std::cout<<"Enter userid "<<std::endl;
            std::cin >> userid;
            User *user = nullptr;
            if (broker->getuserobj(userid)) {
                user = broker->getuserobj(userid);
            }
            if (user == nullptr) {
                std::cout << "User not found for user id:" << userid << std::endl;
                continue;
            }
            std::cout << "Enter password " << std::endl;
            std::cin >> userpassword;
            if (user->verifypassword(userpassword))
            {
                std::cout << "What would you like to do?" << std::endl;
                std::cout << "  1. Manual Order" << std::endl;
                std::cout << "  2. Strategy Order (autoFreeze)" << std::endl;
                int orderType;
                std::cin >> orderType;

                if (orderType == 1)
                {
                    std::string symbol, ordermode;
                    int quantity;
                    double price;
                    std::cout << "Enter order details in (symbol quantity price ordermode): ";
                    std::cin >> symbol >> quantity >> price >> ordermode;
                    try
                    {
                        OrderResult result = om.placeOrder(*user, symbol, quantity, price, ordermode);
                        if (result.wasExecuted && result.executedQty > 0)
                        {
                            user->updateUser(result.executedPrice, result.executedQty, symbol);
                        }
                    }
                    catch (...)
                    {
                        std::cout << "Order placement failed" << std::endl;
                    }
                }
                else if (orderType == 2)
                {
                    std::cout << "Enter details for autoFreezeOrder Strategy in format (symbol quantity price exchangeId orderMode): " << std::endl;
                    std::string symbol, exchangeId, orderMode;
                    long long quantity;
                    double price;
                    std::cin >> symbol >> quantity >> price >> exchangeId >> orderMode;
                    try
                    {
                        auto *stg = new autoFreezeStrategy(symbol, quantity, price, exchangeId, orderMode);
                        MarketInstance.subscribers[symbol].push_back(stg);
                        std::cout << "Strategy Order created successfully" << std::endl;
                    }
                    catch (...)
                    {
                        std::cout << "Strategy Order creation failed" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid option" << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid password for user " << userid << std::endl;
            }
        }
        else if (query == 3)
        {
            bool userlogin = false;
            int userid;
            string userpassword;
            std::cin >> userid >> userpassword;
            User *user = broker->getuserobj(userid);
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
        std::cout << "Enter query" << std::endl;
        std::cin >> query;
    }
    marketThread.join();
    return 0;
}