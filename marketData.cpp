#include "marketData.h"
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "httplib.h"
#include "json.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using json = nlohmann::json;

MarketData& MarketData::getInstance() {
    static MarketData instance;
    return instance;
}

// Define the private constructor for MarketData
MarketData::MarketData() {
    // std::cout<<
}

void MarketData::marketdataevent()
{
    const char* MULTICAST_GROUP = "239.255.0.1";
    const int PORT = 5000;
    const int BUFFER_SIZE = 65536;

    // 1. Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return;
    }

    // 2. Allow multiple listeners
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 3. Bind to port
    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
    perror("bind failed");
    }

    // 4. Join multicast group
    ip_mreq mreq{};
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0) {
        perror("IP_ADD_MEMBERSHIP failed");
        close(sock);
        return;
    }

    std::cout << "[MarketData] Listening on "
              << MULTICAST_GROUP << ":" << PORT << std::endl;
    // 5. Receive loop
    while (true) {
        char buffer[BUFFER_SIZE];
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);

        ssize_t bytes = recvfrom(
            sock,
            buffer,
            sizeof(buffer) - 1,
            0,
            (sockaddr*)&sender,
            &sender_len
        );

        if (bytes <= 0) {
            continue;
        }
        if(bytes > 0){
            buffer[bytes] = '\0';
            // std::cout<<"Received data: "<<buffer<<std::endl;
        }

        try {
            // 6. Parse JSON
            json msg = json::parse(buffer);

            if (msg["type"] != "orders")
                continue;

            // 7. Iterate orders
            for (const auto& order : msg["orders"]) {
                std::string symbol = order["symbol"];
                std::string side   = order["side"];
                int price          = order["price"];
                int quantity       = order["quantity"];

                // 8. Update order book
                updateStrategy(symbol);
                if (side == "BUY") {
                    addBuyOrder(symbol, price, quantity);
                } else if (side == "SELL") {
                    addSellOrder(symbol, price, quantity);
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }

    close(sock);
}


void MarketData::addBuyOrder(const std::string &symbol, int price, int quantity)
{
    std::lock_guard<std::mutex> lock(orderMutex);
    buyOrders[symbol].push({price, quantity});
}

void MarketData::addSellOrder(const std::string &symbol, int price, int quantity)
{
    std::lock_guard<std::mutex> lock(orderMutex);
    sellOrders[symbol].push({price, quantity});
}

std::pair<int, int> MarketData::getBestBuyOrder(const std::string &symbol) const
{
    std::lock_guard<std::mutex> lock(orderMutex);
    if (buyOrders.count(symbol) == 0 || buyOrders.at(symbol).empty())
    {
        return {-1, -1};
    }
    return buyOrders.at(symbol).top();
}

std::pair<int, int> MarketData::getBestSellOrder(const std::string &symbol) const
{
    std::lock_guard<std::mutex> lock(orderMutex);
    if (sellOrders.count(symbol) == 0 || sellOrders.at(symbol).empty())
    {
        return {-1, -1};
    }
    return sellOrders.at(symbol).top();
}

const std::unordered_map<std::string, std::priority_queue<std::pair<int, int>>> &MarketData::getBuyOrders() const
{
    return buyOrders;
}
const std::unordered_map<std::string, std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>>> &MarketData::getSellOrders() const
{
    return sellOrders;
}

void MarketData::updateSellOrder(const std::string &symbol,int price,int quantity){
    std::lock_guard<std::mutex>lock(orderMutex);
    sellOrders[symbol].pop();
    if(quantity>0){
        sellOrders[symbol].push({price,quantity});
    }
}
void MarketData::updateBuyOrder(const std::string &symbol,int price,int quantity){
    std::lock_guard<std::mutex>lock(orderMutex);
    buyOrders[symbol].pop();
    if(quantity>0){
        buyOrders[symbol].push({price,quantity});
    }
}
void MarketData::updateStrategy(const std::string& symbolId) {
    if(subscribers.find(symbolId) != subscribers.end()) {
        for(auto* strategy : subscribers[symbolId]) {
            strategy->onMarketDataUpdate();
        }
    }
}