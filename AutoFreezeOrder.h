#include "strategyBase.h"
#include <iostream>
#include "marketData.h"
#include "orderManager.h"
#ifndef AUTOFREEZEORDER_H
#define AUTOFREEZEORDER_H

class autoFreezeStrategy : public StrategyBase {
public:
   unordered_map<string,singleOrder*>mapOfSingleOrders;
   int size=0;
    autoFreezeStrategy(std::string symbolId,long long qty, double price, const std::string& exchangeId,std::string orderMode);
   long long remainingQty;
    void onMarketDataUpdate() override;
    void start() override;
    void processStrategy() override;

};

#endif // AUTOFREEZEORDER_H