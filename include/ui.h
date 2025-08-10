#ifndef UI_H
#define UI_H

#include "utils.h"
#include "authentication.h"

// Function declarations
void displayPortfolio(User *user, std::map<std::string, std::vector<double>> &closePricesMap,
                      double lastOrderPrice, int screenWidth, int screenHeight, int &lastLineUsed,
                      int &maximumHoldingsCount, int &maximumPendingOrdersCount);

void displayTransactions(const User &user);
void displayInputPrompt();

#endif // UI_H
