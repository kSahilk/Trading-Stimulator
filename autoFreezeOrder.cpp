#include "autoFreezeOrder.h"

autoFreezeStrategy::autoFreezeStrategy(std::string symbolId, long long qty, double price, const std::string &exchangeId, std::string orderMode)
    : StrategyBase()
{
    this->quantity = qty;
    this->price = price;
    this->exchangeId = exchangeId;
    this->orderMode = orderMode;
    this->freezeQty = 20;
    this->symbolId = symbolId;
    std::cout << "Starting auto freeze strategy for " << symbolId << " with quantity " << quantity << " at price " << price << std::endl;
    start();
}
void autoFreezeStrategy::onMarketDataUpdate()
{
    std::cout << "Market data update received for autoFreezeStrategy" << std::endl;
    processStrategy();
}
void autoFreezeStrategy::start()
{
    size = quantity / freezeQty;
    if (quantity % freezeQty != 0)
    {
        remainingQty = quantity % freezeQty;
    }
    for (int i = 0; i < size; i++)
    {
        singleOrder *order = new singleOrder();
        order->symbol = symbolId;
        order->orderMode = orderMode;
        order->price = price;
        order->totalQuantity = freezeQty;
        mapOfSingleOrders[order->unqiueorderClientId] = order;
    }
    if (remainingQty)
    {
        singleOrder *order = new singleOrder();
        order->symbol = symbolId;
        order->orderMode = orderMode;
        order->price = price;
        order->totalQuantity = remainingQty;
        mapOfSingleOrders[order->unqiueorderClientId] = order;
    }
    processStrategy();
}
void autoFreezeStrategy::processStrategy()
{
    // order manager removed; orders manage themselves
    auto it = mapOfSingleOrders.begin();
    for (auto itr = mapOfSingleOrders.begin(); itr != mapOfSingleOrders.end(); itr++)
    {
        if (!itr->second->placeOrder())
        {
            std::cout << "Order with clientId " << itr->second->unqiueorderClientId << " failed to execute. Freezing the order." << std::endl;
            break;
        }
        else
        {
            std::cout << "Order with clientId " << itr->second->unqiueorderClientId << " executed successfully." << std::endl;
            mapOfSingleOrders.erase(itr);
        }
    }
}