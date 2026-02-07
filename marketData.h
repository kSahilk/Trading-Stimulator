#ifndef MARKETDATA_H
#define MARKETDATA_H
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "strategyBase.h"

class MarketData {
private:
    MarketData();
    MarketData(const MarketData&)=delete;
    MarketData& operator=(const MarketData&)=delete;
    std::unordered_map<std::string, std::priority_queue<std::pair<int, int>>> buyOrders;
    std::unordered_map<std::string, std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>>> sellOrders;
    mutable std::mutex orderMutex;
    public:
    std::unordered_map<std::string, std::vector<StrategyBase*>> subscribers;
    static MarketData& getInstance();
    void marketdataevent();
    void updateStrategy(const std::string& symbolId);
    void addBuyOrder(const std::string& symbol, int price, int quantity);
    void addSellOrder(const std::string& symbol, int price, int quantity);
    std::pair<int, int> getBestBuyOrder(const std::string& symbol) const;
    std::pair<int, int> getBestSellOrder(const std::string& symbol) const;
    void updateSellOrder(const std::string &symbol,int price,int quantity);
    void updateBuyOrder(const std::string &symbol,int price,int quantity);
    const std::unordered_map<std::string, std::priority_queue<std::pair<int, int>>>& getBuyOrders() const;
    const std::unordered_map<std::string, std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>>>& getSellOrders() const;
};

#endif // MARKETDATA_H
