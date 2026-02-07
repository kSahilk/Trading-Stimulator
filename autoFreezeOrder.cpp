#include "autoFreezeOrder.h"

autoFreezeStrategy:: autoFreezeStrategy(std::string symbolId,long long qty, double price, const std::string& exchangeId,std::string orderMode)
        : StrategyBase()
    {
        this->quantity = qty;
        this->price = price;
        this->exchangeId = exchangeId;
        this->orderMode = orderMode;
        this->freezeQty = 20;
        this->symbolId = symbolId;
    std::cout<< "Starting auto freeze strategy for " << symbolId << " with quantity " << quantity << " at price " << price << std::endl;
        start();
    }
void autoFreezeStrategy::onMarketDataUpdate()
{
    std::cout<< "Market data update received for " << symbolId << " in strategy " << strategyId << std::endl;
}
void autoFreezeStrategy::start()
{
    
}