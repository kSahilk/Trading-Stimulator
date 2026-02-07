#include "strategyBase.h"
#include <iostream>
#ifndef AUTOFREEZEORDER_H
#define AUTOFREEZEORDER_H

class autoFreezeStrategy : public StrategyBase {
public:
    autoFreezeStrategy(std::string symbolId,long long qty, double price, const std::string& exchangeId,std::string orderMode);
   
    void onMarketDataUpdate() override;
    void start() override;

};

#endif // AUTOFREEZEORDER_H