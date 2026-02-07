#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>
#include <iostream>
#include <algorithm>
#include "user.h"
#include "marketData.h"

using namespace std;

struct OrderResult {
    int executedQty;
    double executedPrice;
    string orderMode;
    bool wasExecuted;
};

class orderManager {
public:
    OrderResult placeOrder(User& user, const string& symbol, int quantity, double price, const string& ordermode) {
        auto& market = MarketData::getInstance();
        int totalTradedQty = 0;
        double totalCost = 0.0;
        bool wasExecuted = false;
        
        if (ordermode == "BUY") {
            while (quantity > 0) {
                auto bestSell = market.getBestSellOrder(symbol);
                if (bestSell.first != -1 && price >= bestSell.first && bestSell.second > 0) {
                    int tradeQty = min(quantity, bestSell.second);
                    if(user.getBalance() < bestSell.first * tradeQty){
                        std::cout<<"Insufficient balance to execute the order"<<std::endl;
                        break;
                    }
                    user.addBalance(-(bestSell.first * tradeQty));
                    totalTradedQty += tradeQty;
                    totalCost += (bestSell.first * tradeQty);
                    user.addToPortfolio(symbol, tradeQty);
                    market.updateSellOrder(symbol, bestSell.first, bestSell.second - tradeQty);
                    quantity -= tradeQty;
                    wasExecuted = true;
                    if (quantity == 0) {
                        std::cout<<"Order Executed Completely with tradedQty: "<< totalTradedQty <<std::endl;
                        break;
                    }
                }
                else {
                    if(totalTradedQty > 0) {
                        std::cout<<"Order executed partially with traded quantity: "<< totalTradedQty <<std::endl;
                    }
                    else {
                        std::cout<<"No matching orders available"<<std::endl;
                    }
                    break;
                }
            }
        }
        else if (ordermode == "SELL") {
            while (quantity > 0) {
                auto bestBuy = market.getBestBuyOrder(symbol);
                if (bestBuy.first != -1 && price <= bestBuy.first && bestBuy.second > 0) {
                    int tradeQty = min(quantity, bestBuy.second);
                    totalTradedQty += tradeQty;
                    totalCost += (bestBuy.first * tradeQty);
                    user.addToPortfolio(symbol, -tradeQty);
                    market.updateBuyOrder(symbol, bestBuy.first, bestBuy.second - tradeQty);
                    user.addBalance(bestBuy.first * tradeQty);
                    quantity -= tradeQty;
                    wasExecuted = true;
                    if (quantity == 0) {
                        std::cout<<"Order executed completely with tradedQty: "<< totalTradedQty <<std::endl;
                        break;
                    }
                } else {
                    if(totalTradedQty > 0) {
                        std::cout<<"Order executed partially with traded quantity: "<< totalTradedQty <<std::endl;
                    }
                    else {
                        std::cout<<"No matching orders available"<<std::endl;
                    }
                    break;
                }
            }
        }
        
        double avgPrice = (totalTradedQty > 0) ? (totalCost / totalTradedQty) : 0.0;
        return {totalTradedQty, avgPrice, ordermode, wasExecuted};
    }

    void cancelOrder() {
        std::cout<<"order cancelled"<<std::endl;
    }

    void modifyOrder() {
        std::cout<<"order modified"<<std::endl;
    }
};

#endif // ORDER_MANAGER_H