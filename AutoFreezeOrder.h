#include "strategyBase.h"
#ifndef AUTOFREEZEORDER_H
#define AUTOFREEZEORDER_H

class autoFreezeStrategy : public StrategyBase {
public:
    autoFreezeStrategy(const std::string& id, long long qty, double price, const std::string& exchangeId,int orderMode)
        : StrategyBase(id) {
        this->quantity = qty;
        this->price = price;
        this->exchangeId = exchangeId;
        this->orderMode = orderMode;
        this->freezeQty = 20; 
    }
    void onMarketDataUpdate() override;

};

#endif // AUTOFREEZEORDER_H