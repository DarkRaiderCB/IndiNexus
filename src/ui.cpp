// src/ui.cpp

#include "utils.h"
#include "ui.h"

void displayPortfolio(User *user, std::map<std::string, std::vector<double>> &closePricesMap,
                      double lastOrderPrice, int screenWidth, int screenHeight, int &lastLineUsed,
                      int &maximumHoldingsCount, int &maximumPendingOrdersCount)
{
    std::lock_guard<std::mutex> dataLock(dataMutex);

    // Update maximumHoldingsCount
    int holdingsCount = user->holdings.size();
    if (holdingsCount > maximumHoldingsCount)
    {
        maximumHoldingsCount = holdingsCount;
    }

    // Update maximumPendingOrdersCount
    int pendingOrdersCount = user->pendingOrders.size();
    if (pendingOrdersCount > maximumPendingOrdersCount)
    {
        maximumPendingOrdersCount = pendingOrdersCount;
    }

    // Calculate the total number of lines needed
    int totalLinesNeeded = 1;                      // For "Portfolio Summary"
    totalLinesNeeded += maximumHoldingsCount;      // For holdings
    totalLinesNeeded += 4;                         // For Total Investment, Current Value, Profit/Loss, Wallet Balance
    totalLinesNeeded += 2;                         // For "Pending Limit Orders:" and spacing
    totalLinesNeeded += maximumPendingOrdersCount; // For pending orders
    int learningTipsLines = 10;                    // Number of lines used by learning tips
    totalLinesNeeded += learningTipsLines + 1;     // For learning tips and spacing

    // Check if totalLinesNeeded exceeds screenHeight
    int portfolioStartY = 1;                 // Default starting line
    if (totalLinesNeeded < screenHeight - 2) // Leave two lines for messages
    {
        portfolioStartY = screenHeight - totalLinesNeeded - 2;
    }

    // Save cursor position
    std::cout << "\033[s";

    // Clear the space where the portfolio will be displayed
    for (int i = 0; i < totalLinesNeeded + 5; ++i)
    {
        moveCursor(1, portfolioStartY + i);
        std::cout << CLEARLINE;
    }

    int currentLine = portfolioStartY;
    moveCursor(1, currentLine++);
    std::cout << "Portfolio Summary for " << user->username << ":";

    double totalInvestment = 0.0;
    double currentValue = 0.0;

    // Display holdings
    for (int i = 0; i < maximumHoldingsCount; ++i)
    {
        moveCursor(1, currentLine++);
        if (i < holdingsCount)
        {
            const auto &holding = user->holdings[i];
            const std::string &holdingSymbol = holding.symbol;
            double latestPrice = 0.0;

            if (!closePricesMap[holdingSymbol].empty())
            {
                latestPrice = closePricesMap[holdingSymbol].back();
            }
            else
            {
                // No price data available for this symbol
                continue;
            }

            double investment = holding.amount * holding.averagePrice;
            double value = holding.amount * latestPrice;

            totalInvestment += investment;
            currentValue += value;

            // Display holding details
            std::cout << CLEARLINE << "Holding: " << holdingSymbol << ", Amount: " << holding.amount
                      << ", Avg Price: INR " << holding.averagePrice << ", Current Price: INR " << latestPrice
                      << ", P/L: INR " << (value - investment);
        }
        else
        {
            // Clear line or output empty line
            std::cout << CLEARLINE;
        }
    }

    double profitLoss = currentValue - totalInvestment;

    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "Total Investment: INR " << totalInvestment;

    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "Current Value: INR " << currentValue;

    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "Profit/Loss: INR " << profitLoss;

    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "Wallet Balance: INR " << user->demoMoney;

    // Show last order price
    if (lastOrderPrice > 0)
    {
        moveCursor(1, currentLine++);
        std::cout << CLEARLINE << "Last Order Price: INR " << lastOrderPrice;
    }
    else
    {
        moveCursor(1, currentLine++);
        std::cout << CLEARLINE; // Clear the line if no last order price
    }

    // Display Pending Limit Orders with indices
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "Pending Limit Orders:";

    for (int i = 0; i < maximumPendingOrdersCount; ++i)
    {
        moveCursor(1, currentLine++);
        if (i < pendingOrdersCount)
        {
            const auto &order = user->pendingOrders[i];
            std::cout << CLEARLINE;
            std::cout << i + 1 << ". " << order.symbol << " - " << order.type << " - Amount: " << order.amount
                      << ", Limit Price: INR " << order.limitPrice;
        }
        else
        {
            // Clear line or output empty line
            std::cout << CLEARLINE;
        }
    }

    if (pendingOrdersCount == 0 && maximumPendingOrdersCount == 0)
    {
        moveCursor(1, currentLine++);
        std::cout << CLEARLINE << "No pending limit orders.";
    }

    currentLine++;

    // Display Learning Tips
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "LEARNING TIPS:";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "1. Buy low, sell high. Avoid buying high and selling low.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "2. Diversify your portfolio to manage risk.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "3. Use limit orders to set your own buy/sell price.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "4. Stay updated with market news and trends.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "5. Remember, the market can remain irrational longer than you can remain solvent.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "6. Always have an exit strategy for your trades.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "7. If RSI is above 80, the market is overbought. If RSI is below 20, the market is oversold.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "8. If a candle and its next crosses moving average above the price, it's a bullish signal. If they cross below, it's bearish.";
    moveCursor(1, currentLine++);
    std::cout << CLEARLINE << "9. A broker fee of 0.05% (max INR 20) applies to each transaction.";

    // Set last line used
    lastLineUsed = currentLine++;

    // Restore cursor position
    std::cout << "\033[u";
    std::cout.flush(); // Ensure output is written immediately
}

// Function to display user transactions
void displayTransactions(const User &user)
{
    // Clear the screen and show cursor
    std::cout << CLEAR_SCREEN << RESET_CURSOR << SHOW_CURSOR;

    std::lock_guard<std::mutex> dataLock(dataMutex);

    std::cout << "=== Your Transactions ===\n\n";

    if (user.transactions.empty())
    {
        std::cout << "No transactions to display.\n";
    }
    else
    {
        std::cout << std::left << std::setw(15) << "Symbol"
                  << std::setw(10) << "Amount"
                  << std::setw(15) << "Price (INR)"
                  << std::setw(10) << "Type"
                  << std::setw(15) << "Broker Fee" << "\n";
        std::cout << "-------------------------------------------------------------\n";

        for (const auto &transaction : user.transactions)
        {
            std::cout << std::left << std::setw(15) << std::get<0>(transaction)
                      << std::setw(10) << std::get<1>(transaction)
                      << std::setw(15) << std::get<2>(transaction)
                      << std::setw(10) << std::get<3>(transaction)
                      << std::setw(15) << std::get<4>(transaction) << "\n";
        }
    }

    std::cout << "\nPress 'm' to return to the main menu...";
    std::cout.flush();

    // Wait for user to press 'm' or 'M'
    char choice;
    do
    {
        choice = std::cin.get();
    } while (choice != 'm' && choice != 'M');
}

// Helper function to display the input prompt
void displayInputPrompt()
{
    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    moveCursor(2, 7);
    std::cout << CLEARLINE << "Enter 'Buy', 'Sell', 'Limit_Buy', 'Limit_Sell', 'Cancel_Limit_Order', 'Help', 'Return_Main_Menu', or 'Exit': ";
    std::cout << SHOW_CURSOR;
    std::cout.flush();
}