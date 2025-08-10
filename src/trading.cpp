// src/trading.cpp

#include "utils.h"
#include "trading.h"
#include "ui.h"

double calculateBrokerFee(double transactionValue)
{
    double feePercentage = transactionValue * 0.0005; // 0.05%
    return std::min(feePercentage, 20.0);             // Maximum fee is 20 rupees
}

bool hasSufficientFunds(User *user, double totalCost)
{
    return user->demoMoney >= totalCost;
}

// Function to check if user has sufficient holdings
bool hasSufficientHoldings(User *user, const std::string &symbol, double amount)
{
    for (const auto &holding : user->holdings)
    {
        if (holding.symbol == symbol && holding.amount >= amount)
        {
            return true;
        }
    }
    return false;
}

// Thread to handle user input
void userInputThread(User *user, std::map<std::string, std::vector<double>> &closePricesMap,
                     std::string &symbol, double &lastOrderPrice)
{
    while (!stopSimulation)
    {
        displayInputPrompt();

        std::string action;
        std::cin >> action;

        {
            std::lock_guard<std::mutex> consoleLock(consoleMutex);
            std::cout << HIDE_CURSOR;
            std::cout.flush();
        }

        // Convert action to lowercase
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);

        if (action == "help")
        {
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                moveCursor(2, 8);
                std::cout << CLEAR_SCREEN;
                moveCursor(2, 2);
                std::cout << "Available commands:";
                moveCursor(2, 3);
                std::cout << "Buy - Buy a specified amount at the current market price.";
                moveCursor(2, 4);
                std::cout << "Sell - Sell a specified amount at the current market price.";
                moveCursor(2, 5);
                std::cout << "Limit_Buy - Place a limit buy order.";
                moveCursor(2, 6);
                std::cout << "Limit_Sell - Place a limit sell order.";
                moveCursor(2, 7);
                std::cout << "Cancel_Limit_Order - Cancel a pending limit order.";
                moveCursor(2, 8);
                std::cout << "Help - Display this help message.";
                moveCursor(2, 9);
                std::cout << "Return_Main_Menu - Return to the main menu to switch stock.";
                moveCursor(2, 10);
                std::cout << "Exit - Exit the trading simulator.";
            }
            // Re-display the input prompt
            moveCursor(2, 7);
            displayInputPrompt();
            continue;
        }

        if (action == "exit")
        {
            stopSimulation = true; // Signal to stop the simulation
            break;
        }
        else if (action == "return_main_menu")
        {
            stopSimulation = true; // Signal to stop the simulation
            changeStock = true;    // Indicate that the user wants to change stock
            break;
        }
        else if (action == "buy" || action == "sell" ||
                 action == "limit_buy" || action == "limit_sell")
        {
            if (closePricesMap[symbol].empty())
            {
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, 8);
                    std::cout << CLEARLINE << "No price data available yet. Please wait...";
                }
                // Re-display the input prompt
                moveCursor(2, 7);
                displayInputPrompt();
                continue;
            }

            double amount;
            double limitPrice = 0.0;
            bool isLimitOrder = false;

            std::string amountStr;
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                moveCursor(2, 8);
                std::cout << CLEARLINE << "Enter amount: ";
                std::cout << SHOW_CURSOR;
                std::cout.flush();
            }
            std::cin >> amountStr;
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                std::cout << HIDE_CURSOR;
            }

            try
            {
                amount = std::stod(amountStr);
                if (amount <= 0)
                {
                    throw std::invalid_argument("Amount must be positive.");
                }
            }
            catch (const std::exception &e)
            {
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, 9);
                    std::cout << CLEARLINE << "Invalid amount entered. Please enter a positive number.";
                }
                // Re-display the input prompt
                moveCursor(2, 7);
                displayInputPrompt();
                continue;
            }

            // Check if it's a limit order
            if (action.find("limit") != std::string::npos)
            {
                isLimitOrder = true;
                std::string limitPriceStr;
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, 9);
                    std::cout << CLEARLINE << "Enter limit price: ";
                    std::cout << SHOW_CURSOR;
                    std::cout.flush();
                }
                std::cin >> limitPriceStr;
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    std::cout << HIDE_CURSOR;
                }

                try
                {
                    limitPrice = std::stod(limitPriceStr);
                    if (limitPrice <= 0)
                    {
                        throw std::invalid_argument("Limit price must be positive.");
                    }
                }
                catch (const std::exception &e)
                {
                    {
                        std::lock_guard<std::mutex> consoleLock(consoleMutex);
                        moveCursor(2, 10);
                        std::cout << CLEARLINE << "Invalid limit price entered. Please enter a positive number.";
                    }
                    // Re-display the input prompt
                    moveCursor(2, 7);
                    displayInputPrompt();
                    continue;
                }
            }

            if (isLimitOrder)
            {
                if (action == "limit_buy")
                {
                    double transactionCost = amount * limitPrice;
                    double estimatedBrokerFee = calculateBrokerFee(transactionCost);
                    double totalCost = transactionCost + estimatedBrokerFee;

                    // Validate funds for limit buy orders
                    if (!hasSufficientFunds(user, totalCost))
                    {
                        {
                            std::lock_guard<std::mutex> consoleLock(consoleMutex);
                            moveCursor(2, 10);
                            std::cout << CLEARLINE << "Insufficient funds to place limit buy order including broker fee.";
                        }
                        // Re-display the input prompt
                        moveCursor(2, 7);
                        displayInputPrompt();
                        continue; // Skip to next iteration
                    }
                }
                else if (action == "limit_sell")
                {
                    // Validate holdings for limit sell orders
                    if (!hasSufficientHoldings(user, symbol, amount))
                    {
                        {
                            std::lock_guard<std::mutex> consoleLock(consoleMutex);
                            moveCursor(2, 10);
                            std::cout << CLEARLINE << "Insufficient holdings to place limit sell order.";
                        }
                        // Re-display the input prompt
                        moveCursor(2, 7);
                        displayInputPrompt();
                        continue; // Skip to next iteration
                    }
                }

                // Create a limit order
                User::Order newOrder;
                newOrder.symbol = symbol;
                newOrder.type = (action == "limit_buy") ? "Limit_Buy" : "Limit_Sell";
                newOrder.amount = amount;
                newOrder.limitPrice = limitPrice;

                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);
                    user->pendingOrders.push_back(newOrder);
                }

                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, 10);
                    std::cout << CLEARLINE << newOrder.type << " order placed for " << amount << " of " << symbol << " at limit price INR " << limitPrice;
                }

                // Re-display the input prompt
                moveCursor(2, 7);
                displayInputPrompt();
            }
            else
            {
                double currentPrice;
                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);
                    currentPrice = closePricesMap[symbol].back();
                }

                if (action == "buy")
                {
                    double transactionCost = amount * currentPrice;
                    double brokerFee = calculateBrokerFee(transactionCost);
                    double totalCost = transactionCost + brokerFee;

                    // Validate funds for buy orders
                    if (!hasSufficientFunds(user, totalCost))
                    {
                        {
                            std::lock_guard<std::mutex> consoleLock(consoleMutex);
                            moveCursor(2, 10);
                            std::cout << CLEARLINE << "Insufficient funds to execute buy order including broker fee.";
                        }
                        // Re-display the input prompt
                        moveCursor(2, 7);
                        displayInputPrompt();
                        continue; // Skip to next iteration
                    }

                    // Market buy order execution
                    {
                        std::lock_guard<std::mutex> dataLock(dataMutex);
                        user->demoMoney -= totalCost;

                        // Update holdings
                        bool found = false;
                        for (auto &holding : user->holdings)
                        {
                            if (holding.symbol == symbol)
                            {
                                // Update average price
                                holding.averagePrice = ((holding.averagePrice * holding.amount) + (currentPrice * amount)) / (holding.amount + amount);
                                holding.amount += amount;
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            // Add new holding
                            User::Holding newHolding;
                            newHolding.symbol = symbol;
                            newHolding.amount = amount;
                            newHolding.averagePrice = currentPrice;
                            user->holdings.push_back(newHolding);
                        }

                        // Update transactions
                        user->transactions.push_back({symbol, amount, currentPrice, std::string("Buy"), brokerFee});
                    }

                    // After processing the order, set the lastOrderPrice to the current price
                    {
                        std::lock_guard<std::mutex> dataLock(dataMutex);
                        lastOrderPrice = currentPrice; // Update the last order price
                    }

                    {
                        std::lock_guard<std::mutex> consoleLock(consoleMutex);
                        moveCursor(2, 9);
                        std::cout << CLEARLINE << "Buy order placed for " << amount << " of " << symbol << " at INR " << currentPrice;
                        moveCursor(2, 10);
                        std::cout << CLEARLINE << "Broker Fee: INR " << brokerFee << ", Total Cost: INR " << totalCost;
                    }

                    // Re-display the input prompt
                    moveCursor(2, 7);
                    displayInputPrompt();
                }
                else if (action == "sell")
                {
                    // Validate holdings for sell orders
                    if (!hasSufficientHoldings(user, symbol, amount))
                    {
                        {
                            std::lock_guard<std::mutex> consoleLock(consoleMutex);
                            moveCursor(2, 10);
                            std::cout << CLEARLINE << "Insufficient holdings to execute sell order.";
                        }
                        // Re-display the input prompt
                        moveCursor(2, 7);
                        displayInputPrompt();
                        continue; // Skip to next iteration
                    }

                    double transactionValue = amount * currentPrice;
                    double brokerFee = calculateBrokerFee(transactionValue);
                    double netProceeds = transactionValue - brokerFee;

                    // Market sell order execution
                    {
                        std::lock_guard<std::mutex> dataLock(dataMutex);
                        user->demoMoney += netProceeds;

                        // Update holdings
                        for (auto it = user->holdings.begin(); it != user->holdings.end(); ++it)
                        {
                            if (it->symbol == symbol)
                            {
                                it->amount -= amount;
                                if (it->amount <= 0)
                                {
                                    // Remove holding from vector
                                    it = user->holdings.erase(it);
                                }
                                // No need to update 'it' because we break immediately
                                break;
                            }
                        }

                        // Update transactions
                        user->transactions.push_back({symbol, amount, currentPrice, std::string("Sell"), brokerFee});
                    }

                    // After processing the order, set the lastOrderPrice to the current price
                    {
                        std::lock_guard<std::mutex> dataLock(dataMutex);
                        lastOrderPrice = currentPrice; // Update the last order price
                    }

                    {
                        std::lock_guard<std::mutex> consoleLock(consoleMutex);
                        moveCursor(2, 9);
                        std::cout << CLEARLINE << "Sell order executed for " << amount << " of " << symbol << " at INR " << currentPrice;
                        moveCursor(2, 10);
                        std::cout << CLEARLINE << "Broker Fee: INR " << brokerFee << ", Net Proceeds: INR " << netProceeds;
                    }

                    // Re-display the input prompt
                    moveCursor(2, 7);
                    displayInputPrompt();
                }
            }
        }
        else if (action == "cancel_limit_order")
        {
            bool hasOrders = false;
            std::vector<User::Order> ordersCopy;

            // Check if there are pending orders and make a copy
            {
                std::lock_guard<std::mutex> dataLock(dataMutex);
                hasOrders = !user->pendingOrders.empty();
                if (hasOrders)
                {
                    ordersCopy = user->pendingOrders; // Copy the orders
                }
            }

            if (!hasOrders)
            {
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, 8);
                    std::cout << CLEARLINE << "No pending limit orders to cancel.";
                }
                // Re-display the input prompt
                moveCursor(2, 7);
                displayInputPrompt();
                continue;
            }

            // Display pending limit orders with indices
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                moveCursor(2, 8);
                std::cout << CLEARLINE << "Your Pending Limit Orders:";
            }
            int index = 1;
            int displayLine = 9;
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                for (const auto &order : ordersCopy)
                {
                    moveCursor(2, displayLine++);
                    std::cout << CLEARLINE << index << ". " << order.symbol << " - " << order.type << " - Amount: " << order.amount << ", Limit Price: INR " << order.limitPrice;
                    index++;
                }
                moveCursor(2, displayLine);
                std::cout << CLEARLINE << "Enter the number of the order you wish to cancel: ";
                std::cout << SHOW_CURSOR;
                std::cout.flush();
            }
            std::vector<User::Order>::size_type cancelIndex;
            std::cin >> cancelIndex;
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                std::cout << HIDE_CURSOR;
            }

            if (cancelIndex >= 1 && cancelIndex <= ordersCopy.size())
            {
                // Remove the selected order
                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);
                    if (cancelIndex <= user->pendingOrders.size())
                    {
                        user->pendingOrders.erase(user->pendingOrders.begin() + cancelIndex - 1);
                    }
                }
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, displayLine + 1);
                    std::cout << CLEARLINE << "Limit order #" << cancelIndex << " has been canceled.";
                }
            }
            else
            {
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(2, displayLine + 1);
                    std::cout << CLEARLINE << "Invalid order number.";
                }
            }

            // Re-display the input prompt
            moveCursor(2, 7);
            displayInputPrompt();
        }
        else
        {
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                moveCursor(2, 9);
                std::cout << CLEARLINE << "Invalid input. Please enter 'Buy', 'Sell', 'Limit_Buy', 'Limit_Sell', 'Cancel_Limit_Order', 'Help', 'Return_Main_Menu', or 'Exit'.";
            }
            // Re-display the input prompt
            moveCursor(2, 7);
            displayInputPrompt();
        }
    }
}