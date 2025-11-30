#include "marketData.h"
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

MarketData& MarketData::getInstance() {
    static MarketData instance;
    return instance;
}

// Define the private constructor for MarketData
MarketData::MarketData() {
    // std::cout<<
}

void MarketData::marketdataevent(const std::vector<std::string>& symbols) {
    httplib::Client client("http://www.alphavantage.co");

    while (true)
    {
        for (const auto &symbol : symbols)
        {
            std::string endpoint = "/query?function=TIME_SERIES_INTRADAY&symbol=" + symbol +
                                   "&interval=5min&apikey=WR3VQBGKWJO8X9Y8&outputsize=compact";

            auto res = client.Get(endpoint.c_str());

            if (!res || res->status != 200)
            {
                std::cout << "API call failed for " << symbol << ", retrying...\n";
                continue;
            }

            json data = json::parse(res->body);

            if (!data.contains("Time Series (5min)"))
            {
                std::cout << "No time series data for " << symbol << "\n";
                continue;
            }

            auto timeSeries = data["Time Series (5min)"];

            for (auto &item : timeSeries.items())
            {
                auto candle = item.value();
                double open = std::stod(candle["1. open"].get<std::string>());
                double high = std::stod(candle["2. high"].get<std::string>());
                double low = std::stod(candle["3. low"].get<std::string>());
                double close = std::stod(candle["4. close"].get<std::string>());

                double midPrice = (open + close) / 2.0;
                double spread = (high - low) / 4.0;
                if (spread == 0)
                    spread = 1;

                {
                    std::lock_guard<std::mutex> lock(orderMutex);

                    buyOrders[symbol].push({static_cast<int>(midPrice), 200});
                    buyOrders[symbol].push({static_cast<int>(midPrice - spread), 100});
                    buyOrders[symbol].push({static_cast<int>(midPrice - 2 * spread), 50});

                    sellOrders[symbol].push({static_cast<int>(midPrice + spread), 200});
                    sellOrders[symbol].push({static_cast<int>(midPrice + 2 * spread), 100});
                    sellOrders[symbol].push({static_cast<int>(midPrice + 3 * spread), 50});
                }
            }

            // Sleep a bit to avoid hitting API limits per symbol
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Sleep for a longer interval before the next batch to respect AlphaVantage free API limit
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

void MarketData::addBuyOrder(const string &symbol, int price, int quantity)
{
    std::lock_guard<std::mutex> lock(orderMutex);
    buyOrders[symbol].push({price, quantity});
}

void MarketData::addSellOrder(const string &symbol, int price, int quantity)
{
    std::lock_guard<std::mutex> lock(orderMutex);
    sellOrders[symbol].push({price, quantity});
}

pair<int, int> MarketData::getBestBuyOrder(const string &symbol) const
{
    std::lock_guard<std::mutex> lock(orderMutex);
    if (buyOrders.count(symbol) == 0 || buyOrders.at(symbol).empty())
    {
        return {-1, -1};
    }
    return buyOrders.at(symbol).top();
}

pair<int, int> MarketData::getBestSellOrder(const string &symbol) const
{
    std::lock_guard<std::mutex> lock(orderMutex);
    if (sellOrders.count(symbol) == 0 || sellOrders.at(symbol).empty())
    {
        return {-1, -1};
    }
    return sellOrders.at(symbol).top();
}

const unordered_map<string, priority_queue<pair<int, int>>> &MarketData::getBuyOrders() const
{
    return buyOrders;
}
const unordered_map<string, priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>> &MarketData::getSellOrders() const
{
    return sellOrders;
}