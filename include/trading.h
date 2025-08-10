#ifndef TRADING_H
#define TRADING_H

#include "utils.h"
#include "data_management.h"
#include "data_persistence.h"

// Function declarations
double calculateBrokerFee(double transactionValue);
bool hasSufficientFunds(User *user, double totalCost);
bool hasSufficientHoldings(User *user, const std::string &symbol, double amount);
void userInputThread(User *user, std::map<std::string, std::vector<double>> &closePricesMap, std::string &symbol, double &lastOrderPrice);

#endif // TRADING_H
