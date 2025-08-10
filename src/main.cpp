// src/main.cpp

#include "utils.h"
#include "ui.h"
#include "authentication.h"
#include "data_management.h"
#include "trading.h"
#include "simulations.h"
#include "visualization.h"
#include "data_persistence.h"

// Mutexes for synchronization
std::mutex dataMutex;                    // Mutex for data synchronization
std::mutex consoleMutex;                 // Mutex for console output synchronization
std::atomic<bool> stopSimulation(false); // To stop the simulation
std::atomic<bool> changeStock(false);    // To change the stock
std::atomic<bool> inputReceived(false);  // Flag to indicate input has been received

// Map of initial prices and volatility for each asset
std::map<std::string, std::pair<double, double>> assetData = {
    {"RELYCORP", {2600.00, 1.00}},
    {"TECHSOL", {4200.00, 1.50}},
    {"INFOWAVE", {1800.00, 1.00}},
    {"NDFBANK", {1600.00, 0.50}},
    {"FMCGUNION", {2800.00, 0.80}},
    {"METALWORKS", {160.00, 1.20}},
    {"SAFEBANK", {770.00, 0.60}},
};

// Updated main function
int main()
{
#ifdef _WIN32
    EnableVirtualTerminalProcessing(); // Enable ANSI escape codes
#endif

    User user;

    int option;
    std::cout << "1. Signup\n2. Login\nEnter option: ";
    std::cin >> option;

    if (option == 1)
    {
        if (!signup(user))
        {
            std::cout << "Signup failed." << std::endl;
            return 1;
        }
        else
        {
            std::cout << "Signup successful. Please log in to continue." << std::endl;
            // Redirect to login
            if (!login(user))
            {
                std::cout << "Login failed." << std::endl;
                return 1;
            }
        }
    }
    else if (option == 2)
    {
        if (!login(user))
        {
            std::cout << "Login failed." << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Invalid option. Exiting." << std::endl;
        return 1;
    }

    // Load stock data from disk if available
    loadStockData(closePricesMap, candlesMap);

    // Start real-time simulations for all symbols in separate threads
    std::vector<std::thread> simulationThreads;
    for (const auto &pair : assetData)
    {
        const std::string &simSymbol = pair.first;
        stopFlags[simSymbol] = false;
        // Initialize vectors if not loaded
        if (closePricesMap.find(simSymbol) == closePricesMap.end())
            closePricesMap[simSymbol] = std::vector<double>();
        if (candlesMap.find(simSymbol) == candlesMap.end())
            candlesMap[simSymbol] = std::vector<Candle>();
        simulationThreads.emplace_back(simulateSymbolPrice, simSymbol, std::ref(closePricesMap[simSymbol]),
                                       std::ref(candlesMap[simSymbol]), std::ref(stopFlags[simSymbol]));
    }

    // Terminal control
    std::cout << CLEAR_SCREEN << HIDE_CURSOR;

    // Get terminal screen width and height
    int screenWidth = 80;  // Typical terminal width; adjust if necessary
    int screenHeight = 24; // Typical terminal height; adjust if necessary

    GetConsoleSize(screenWidth, screenHeight);

    bool exitProgram = false;
    int maximumHoldingsCount = 0;
    int maximumPendingOrdersCount = 0;

    int lastLineUsed = 0; // To keep track of the last line used in the portfolio display

    while (!exitProgram)
    {
        stopSimulation = false;      // Reset the stopSimulation flag
        changeStock = false;         // Reset the changeStock flag
        double lastOrderPrice = 0.0; // Reset lastOrderPrice

        {
            std::lock_guard<std::mutex> consoleLock(consoleMutex);
            std::cout << CLEAR_SCREEN << HIDE_CURSOR;
            moveCursor(2, 2);
            std::cout << "Welcome to IndiNexus - Learn Indian Equity Markets with Realistic Simulations, " << user.username << "!";
            moveCursor(2, 3);
            std::cout << "Available assets:";
            moveCursor(2, 4);
            std::cout << "RELYCORP, TECHSOL, INFOWAVE, NDFBANK, FMCGUNION, METALWORKS, SAFEBANK";
            moveCursor(2, 5);
            std::cout << "Enter the stock you want to trade, or type 'transactions' to view your transactions, or 'exit' to quit: ";
            std::cout << SHOW_CURSOR;
            std::cout.flush();
        }
        // Ask for the stock
        std::string input;
        std::cin >> input;
        {
            std::lock_guard<std::mutex> consoleLock(consoleMutex);
            std::cout << HIDE_CURSOR;
            std::cout.flush();
        }

        // Convert symbol to uppercase
        std::string inputUpper = input;
        std::transform(inputUpper.begin(), inputUpper.end(), inputUpper.begin(), ::toupper);

        if (inputUpper == "TRANSACTIONS")
        {
            displayTransactions(user);
            continue; // Return to the start of the loop
        }
        else if (inputUpper == "EXIT")
        {
            stopSimulation = true; // Signal to stop the simulation
            exitProgram = true;    // Set exit flag
            break;
        }
        else
        {
            // Validate symbol
            std::string symbol = inputUpper;
            std::vector<std::string> validSymbols = {"RELYCORP", "TECHSOL", "INFOWAVE", "NDFBANK", "FMCGUNION",
                                                     "METALWORKS", "SAFEBANK"};
            if (std::find(validSymbols.begin(), validSymbols.end(), symbol) == validSymbols.end())
            {
                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                moveCursor(2, 6);
                std::cout << "Invalid stock name. Please try again.";
                std::cout.flush();
                continue; // Return to the start of the loop
            }

            // Start user input handling in a separate thread
            std::thread inputThread(userInputThread, &user, std::ref(closePricesMap), std::ref(symbol), std::ref(lastOrderPrice));

            // Open gnuplot pipe and redirect output to NUL to suppress messages
#ifdef _WIN32
#define GNUPLOT_PATH "gnuplot > NUL 2>&1"
#else
#define GNUPLOT_PATH "gnuplot > /dev/null 2>&1"
#endif

            FILE *gnuplotPipe = popen(GNUPLOT_PATH, "w");
            if (gnuplotPipe == NULL)
            {
                std::cerr << "Error: Could not open gnuplot pipe." << std::endl;
                return 1;
            }

            // Set the terminal type once at the beginning
            fprintf(gnuplotPipe, "reset\n");
#ifdef _WIN32
            fprintf(gnuplotPipe, "set term windows\n"); // Use 'windows' terminal
#else
            fprintf(gnuplotPipe, "set term %s\n", "qt"); // Use 'qt' terminal
#endif

            // Reset lastLineUsed for the new simulation
            lastLineUsed = 0;

            // Main loop: Update portfolio display, execute limit orders, and plot every second
            while (!stopSimulation)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));

                // Update portfolio display
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    displayPortfolio(&user, closePricesMap, lastOrderPrice, screenWidth, screenHeight,
                                     lastLineUsed, maximumHoldingsCount, maximumPendingOrdersCount);

                    std::cout.flush();
                }

                // Check and execute pending limit orders
                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);
                    // Iterate through a copy of pendingOrders to safely remove executed orders
                    std::vector<User::Order> executedOrders;
                    for (const auto &order : user.pendingOrders)
                    {
                        // Get current price for order.symbol
                        double currentPrice = 0.0;
                        if (!closePricesMap[order.symbol].empty())
                        {
                            currentPrice = closePricesMap[order.symbol].back();
                        }
                        else
                        {
                            // No price data available for this symbol
                            continue;
                        }

                        if (order.type == "Limit_Buy" && currentPrice <= order.limitPrice)
                        {
                            // Check if user has sufficient funds
                            if (!hasSufficientFunds(&user, order.amount * currentPrice))
                            {
                                continue; // Skip to next order
                            }

                            // Execute Limit Buy
                            user.transactions.emplace_back(order.symbol, order.amount, currentPrice, "Limit_Buy", calculateBrokerFee(order.amount * currentPrice));
                            user.demoMoney -= order.amount * currentPrice;

                            // Update holdings
                            bool found = false;
                            for (auto &holding : user.holdings)
                            {
                                if (holding.symbol == order.symbol)
                                {
                                    // Update average price
                                    holding.averagePrice = ((holding.averagePrice * holding.amount) + (currentPrice * order.amount)) / (holding.amount + order.amount);
                                    holding.amount += order.amount;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                // Add new holding
                                User::Holding newHolding;
                                newHolding.symbol = order.symbol;
                                newHolding.amount = order.amount;
                                newHolding.averagePrice = currentPrice;
                                user.holdings.push_back(newHolding);
                            }

                            lastOrderPrice = currentPrice;
                            executedOrders.push_back(order);

                            // Inform the user
                            {
                                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                                moveCursor(2, 9);
                                std::cout << CLEARLINE << "Limit_Buy order executed for " << order.amount << " of " << order.symbol << " at INR " << currentPrice;
                                moveCursor(2, 7);
                            }
                            displayInputPrompt();
                        }
                        else if (order.type == "Limit_Sell" && currentPrice >= order.limitPrice)
                        {
                            // Check if user has sufficient holdings
                            if (!hasSufficientHoldings(&user, order.symbol, order.amount))
                            {
                                continue; // Skip to next order
                            }

                            // Execute Limit Sell
                            user.transactions.emplace_back(order.symbol, order.amount, currentPrice, "Limit_Sell", calculateBrokerFee(order.amount * currentPrice));
                            user.demoMoney += order.amount * currentPrice;

                            // Update holdings
                            for (auto it = user.holdings.begin(); it != user.holdings.end(); ++it)
                            {
                                if (it->symbol == order.symbol)
                                {
                                    it->amount -= order.amount;
                                    if (it->amount <= 0)
                                    {
                                        // Remove holding from vector
                                        it = user.holdings.erase(it);
                                    }
                                    // No need to update 'it' because we break immediately
                                    break;
                                }
                            }

                            lastOrderPrice = currentPrice;
                            executedOrders.push_back(order);

                            // Inform the user
                            {
                                std::lock_guard<std::mutex> consoleLock(consoleMutex);
                                moveCursor(2, 9);
                                std::cout << CLEARLINE << "Limit_Sell order executed for " << order.amount << " of " << order.symbol << " at INR " << currentPrice;
                                moveCursor(2, 7);
                            }
                            displayInputPrompt();
                        }
                    }

                    // Remove executed orders from pendingOrders
                    for (const auto &executedOrder : executedOrders)
                    {
                        auto it = std::find_if(user.pendingOrders.begin(), user.pendingOrders.end(),
                                               [&executedOrder](const User::Order &o)
                                               {
                                                   return o.symbol == executedOrder.symbol &&
                                                          o.type == executedOrder.type &&
                                                          o.amount == executedOrder.amount &&
                                                          o.limitPrice == executedOrder.limitPrice;
                                               });
                        if (it != user.pendingOrders.end())
                        {
                            user.pendingOrders.erase(it);
                        }
                    }
                }

                // Write data files for plotting
                std::vector<Candle> candles;
                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);

                    // Check if there is price data for the current symbol
                    if (candlesMap[symbol].empty())
                        continue;

                    candles = candlesMap[symbol];

                    // Write candles data
                    std::ofstream candlesFile("data/candles.dat");
                    for (size_t i = 0; i < candles.size(); ++i)
                    {
                        candlesFile << i << " " << candles[i].open << " " << candles[i].high << " " << candles[i].low << " " << candles[i].close << "\n";
                    }
                    candlesFile.close();

                    // Calculate moving average
                    const int maPeriod = 5;
                    std::ofstream maFile("data/moving_average.dat");
                    for (size_t i = 0; i < candles.size(); ++i)
                    {
                        double ma = calculateMovingAverage(candles, maPeriod, i);
                        maFile << i << " " << ma << "\n";
                    }
                    maFile.close();

                    // Calculate RSI
                    const int rsiPeriod = 14;
                    std::ofstream rsiFile("data/rsi.dat");
                    for (size_t i = 1; i < candles.size(); ++i)
                    {
                        double rsi = calculateRSI(candles, rsiPeriod, i);
                        rsiFile << i << " " << rsi << "\n";
                    }
                    rsiFile.close();
                }

                // Calculate the minimum and maximum price from candles
                double minPrice = std::numeric_limits<double>::max();
                double maxPrice = std::numeric_limits<double>::lowest();

                for (const auto &candle : candles)
                {
                    if (candle.low < minPrice)
                        minPrice = candle.low;
                    if (candle.high > maxPrice)
                        maxPrice = candle.high;
                }

                // Add some padding to the min and max prices for better visualization
                double padding = (maxPrice - minPrice) * 0.05;
                minPrice -= padding;
                maxPrice += padding;

                // Calculate x-range to focus on recent candles
                size_t numCandles = candles.size();
                size_t xrange_min = numCandles >= 50 ? numCandles - 50 : 0; // Display last 50 candles
                size_t xrange_max = numCandles - 1;

                // Send Gnuplot commands directly
                fprintf(gnuplotPipe, "reset\n");
                fprintf(gnuplotPipe, "set xrange [%zu:%zu]\n", xrange_min, xrange_max);
                fprintf(gnuplotPipe, "set yrange [%f:%f]\n", minPrice, maxPrice); // Set yrange
                fprintf(gnuplotPipe, "set style fill solid 0.4\n");               // Filled candlestick body
                fprintf(gnuplotPipe, "set grid\n");

                // Begin multiplot
                fprintf(gnuplotPipe, "set multiplot title 'IndiNexus - %s'\n", symbol.c_str());

                // First plot: Candlestick chart with Moving Average
                fprintf(gnuplotPipe, "set size 1.0,0.75\n");
                fprintf(gnuplotPipe, "set origin 0.0,0.2\n");        // Starting from y=0.2 to y=1.0
                fprintf(gnuplotPipe, "set boxwidth 0.3 relative\n"); // Adjust boxwidth to make candles thinner
                fprintf(gnuplotPipe, "set style data candlesticks\n");

                // Optional: Set the legend/key position for the candlestick chart
                fprintf(gnuplotPipe, "set key top right\n"); // You can customize the position (e.g., "top right", "bottom left")

                // Annotate the current price at the bottom right corner of the candlestick chart
                {
                    std::lock_guard<std::mutex> dataLock(dataMutex);
                    if (!closePricesMap[symbol].empty())
                    {
                        double currentPrice = closePricesMap[symbol].back();
                        // Place the label after setting size and origin
                        fprintf(gnuplotPipe, "set label 1 'Current Price: INR %.2f' at graph 0.99, graph 0.04 right front\n", currentPrice);
                    }
                }

                // Plot the candlestick chart and moving average with titles for the legend
                fprintf(gnuplotPipe, "plot \\\n");
                fprintf(gnuplotPipe, "'data/candles.dat' using 1:($5>$2?$2:1):($5>$2?$3:1):($5>$2?$4:1):($5>$2?$5:1) with candlesticks lw 1 lc rgb 'green' title 'Bullish', \\\n");
                fprintf(gnuplotPipe, "'data/candles.dat' using 1:($5<=$2?$2:1):($5<=$2?$3:1):($5<=$2?$4:1):($5<=$2?$5:1) with candlesticks lw 1 lc rgb 'red' title 'Bearish', \\\n");
                fprintf(gnuplotPipe, "'data/moving_average.dat' using 1:2 with lines lw 2 lc rgb 'blue' title 'Moving Average'\n");

                // Unset the label after the first plot
                fprintf(gnuplotPipe, "unset label 1\n");

                // Second plot: RSI
                fprintf(gnuplotPipe, "set size 1.0,0.2\n");
                fprintf(gnuplotPipe, "set origin 0.0,0.0\n"); // Starting from y=0.0 to y=0.2
                fprintf(gnuplotPipe, "set grid\n");
                fprintf(gnuplotPipe, "set yrange [0:100]\n");
                fprintf(gnuplotPipe, "set key autotitle columnheader\n"); // This is why RSI legend appears
                fprintf(gnuplotPipe, "plot \\\n");
                fprintf(gnuplotPipe, "'data/rsi.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'RSI', \\\n");
                fprintf(gnuplotPipe, "80 with lines dt 2 lw 3 lc rgb 'gray' notitle, \\\n");
                fprintf(gnuplotPipe, "20 with lines dt 2 lw 3 lc rgb 'gray' notitle\n");

                // Finish multiplot
                fprintf(gnuplotPipe, "unset multiplot\n");
                fflush(gnuplotPipe);
                std::cout.flush();
            }

            // Close gnuplot pipe
            pclose(gnuplotPipe);

            // Wait for user input thread to finish
            if (inputThread.joinable())
            {
                inputThread.join();
            }

            // If the user wants to change stock, continue
            if (changeStock)
            {
                // Save user data before changing stock
                user.saveUserData();
                // Inform the user
                {
                    std::lock_guard<std::mutex> consoleLock(consoleMutex);
                    moveCursor(1, lastLineUsed + 1);
                    std::cout << "Returning to main menu..." << std::endl;
                    std::cout.flush();
                }
                continue; // Loop back to main menu
            }
            else
            {
                // User chose to exit
                exitProgram = true;
            }
        }
    }

    // Display exit messages after the portfolio section
    {
        std::lock_guard<std::mutex> consoleLock(consoleMutex);
        moveCursor(1, lastLineUsed + 1);
        std::cout << "Exiting trading interface..." << std::endl;
        moveCursor(1, lastLineUsed + 2);
        std::cout << "Thank you for using IndiNexus!" << std::endl;
        std::cout << SHOW_CURSOR;
    }

    // Save user data before exiting
    user.saveUserData();

    // Save stock data before exiting
    saveStockData(closePricesMap, candlesMap);

    // Signal all simulation threads to stop
    for (auto &pair : stopFlags)
    {
        pair.second = true;
    }

    // Wait for simulation threads to finish
    for (auto &simThread : simulationThreads)
    {
        if (simThread.joinable())
        {
            simThread.join();
        }
    }

    return 0;
}
