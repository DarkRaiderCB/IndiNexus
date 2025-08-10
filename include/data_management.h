#ifndef DATA_MANAGEMENT_H
#define DATA_MANAGEMENT_H

#include "utils.h"


struct User
{
    std::string fullName;
    std::string username;
    std::string password; // Changed from password to password
    double demoMoney;
    double initialDemoMoney;
    std::vector<std::tuple<std::string, double, double, std::string, double>> transactions; // symbol, amount, price, type, brokerFee

    // Structure to represent a holding
    struct Holding
    {
        std::string symbol;
        double amount;
        double averagePrice;
    };
    std::vector<Holding> holdings; // Track holdings

    // Structure for pending limit orders
    struct Order
    {
        std::string symbol;
        std::string type; // "Limit_Buy" or "Limit_Sell"
        double amount;
        double limitPrice;
    };
    std::vector<Order> pendingOrders; // Pending limit orders

    void saveUserData();
    bool loadUserData();
};

#endif // DATA_MANAGEMENT_H
