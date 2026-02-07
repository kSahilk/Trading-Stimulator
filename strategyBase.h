#include <string>
#include <unordered_map>
#include "marketData.h"
#ifndef STRATEGYBASE_H
#define STRATEGYBASE_H

class StrategyBase {
protected:
    std::string strategyId;
    std::string symbolId;
    long long quantity;
    double price;
    std::string orderMode; 
    std::string exchangeId;
    long long freezeQty;
    bool isActive;
    long long tradedQty;
public:
    StrategyBase() : strategyId(generateStgId()), isActive(true), tradedQty(0) {}
    virtual void onMarketDataUpdate() = 0;
    virtual void start() = 0;
    virtual ~StrategyBase() {}
    static std::string generateStgId() {
        static int counter = 0;
        return "STG" + std::to_string(++counter);
    }
};
#endif // STRATEGYBASE_H