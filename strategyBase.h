#include <string>
#include <unordered_map>

class StrategyBase {
protected:
    std::string strategyId;
    std::string SymbolId;
    long long quantity;
    long long price;
    int orderMode;
    std::string exchangeId;
    long long freezeQty;
    bool isActive;
    long long tradedQty;
public:
    StrategyBase(const std::string& id) : strategyId(id), isActive(true), tradedQty(0) {}
    virtual void onMarketDataUpdate() = 0;
    virtual ~StrategyBase() {}
};