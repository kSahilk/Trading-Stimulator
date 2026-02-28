#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>
#include <iostream>
#include <algorithm>
#include <math.h>
#include "user.h"
#include "marketData.h"
#include "enums.h"
#include <random>

using namespace std;

inline string generateUniqueClientId() {
    static const string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, chars.size() - 1);

    string clientId;
    for (int i = 0; i < 10; i++) {
        clientId += chars[dist(gen)];
    }

    return clientId;
}



struct OrderResult {
    int executedQty;
    double executedPrice;
    string orderMode;
    bool wasExecuted;
};

// simple POD representing an order; fields are public so callers can construct and modify
struct singleOrder {
    string orderMode;
    long long tradedQuantity;
    long long totalQuantity;
    OrderStatus orderStatus;
    long long price;
    long long overprice;
    string symbol;
    string unqiueorderClientId;

    // public default ctor to allow callers to allocate and populate the order
    singleOrder() {
        unqiueorderClientId = generateUniqueClientId();
        overprice = 0;
        tradedQuantity = 0;
        totalQuantity = 0;
        price = 0;
        orderStatus = orderStatus_confirmed; // default placeholder
    }

    // attempt to match the order against the market; any execution returns true
    bool placeOrder() {
        auto &market = MarketData::getInstance();
        bool wasExecuted = false;

        if (orderMode == "BUY") {
            while (totalQuantity > 0) {
                auto bestSell = market.getBestSellOrder(symbol);
                if (bestSell.first != -1 && price >= bestSell.first && bestSell.second > 0) {
                    int tradeQty = std::min(totalQuantity, bestSell.second);
                    market.updateSellOrder(symbol, bestSell.first, bestSell.second - tradeQty);
                    totalQuantity -= tradeQty;
                    wasExecuted = true;
                } else {
                    break;
                }
            }
        } else if (orderMode == "SELL") {
            while (totalQuantity > 0) {
                auto bestBuy = market.getBestBuyOrder(symbol);
                if (bestBuy.first != -1 && price <= bestBuy.first && bestBuy.second > 0) {
                    int tradeQty = std::min(totalQuantity, bestBuy.second);
                    market.updateBuyOrder(symbol, bestBuy.first, bestBuy.second - tradeQty);
                    totalQuantity -= tradeQty;
                    wasExecuted = true;
                } else {
                    break;
                }
            }
        }

        return wasExecuted;
    }

    void cancelOrder() {
        std::cout << "order cancelled" << std::endl;
    }

    void modifyOrder() {
        std::cout << "order modified" << std::endl;
    }
};


#endif // ORDER_MANAGER_H