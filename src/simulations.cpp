// src/simulation.cpp

#include "utils.h"
#include "simulations.h"
#include "data_persistence.h"

// Function to simulate real-time price updates for a single symbol
void simulateSymbolPrice(const std::string &symbol, std::vector<double> &closePrices, std::vector<Candle> &candles, std::atomic<bool> &stopFlag)
{
    // Retrieve initial price and volatility for the symbol
    double initialPrice = assetData[symbol].first;
    double volatility = assetData[symbol].second;

    // Seed the generator based on the symbol for consistency
    std::mt19937 gen(std::hash<std::string>{}(symbol));
    std::normal_distribution<> normDist(0.0, 1.0);

    double dt = 1.0 / 60.0; // Assume 60 time steps per minute

    // Define candle interval in seconds
    const int candleInterval = 10; // Each candle represents 10 seconds

    // Initialize price
    double price = initialPrice;

    // Pre-load at least 50 candles before starting the live simulation
    const int preLoadCandles = 50;
    const int preLoadSeconds = preLoadCandles * candleInterval;

    // Initialize candle variables
    double openPrice = price;
    double highPrice = price;
    double lowPrice = price;
    double closePrice = price;
    int secondCounter = 0;

    // Pre-load data
    for (int i = 0; i < preLoadSeconds; ++i)
    {
        // Generate a new price point
        double randStdNormal = normDist(gen);
        double changePercent = volatility * sqrt(dt) * randStdNormal / 100.0; // Convert volatility to a percentage

        // Constrain changePercent to prevent underflow/overflow
        const double maxChangePercent = 0.1; // Maximum 10% change per time step
        if (changePercent > maxChangePercent)
            changePercent = maxChangePercent;
        else if (changePercent < -maxChangePercent)
            changePercent = -maxChangePercent;

        price = price * exp(changePercent);

        // Ensure price does not become zero or negative
        if (price < 0.01)
            price = 0.01;

        // Update price data
        {
            std::lock_guard<std::mutex> dataLock(dataMutex);
            closePrices.push_back(price);
        }

        // Update candle data
        if (secondCounter == 0)
        {
            openPrice = price;
            highPrice = price;
            lowPrice = price;
        }
        else
        {
            if (price > highPrice)
                highPrice = price;
            if (price < lowPrice)
                lowPrice = price;
        }

        closePrice = price;
        secondCounter++;

        if (secondCounter >= candleInterval)
        {
            // Aggregate into a candle
            Candle candle = {openPrice, highPrice, lowPrice, closePrice};
            {
                std::lock_guard<std::mutex> dataLock(dataMutex);
                candles.push_back(candle);
            }

            // Save the candle to disk
            saveCandleToDisk(symbol, candle);

            // Reset for the next interval
            openPrice = price;
            highPrice = price;
            lowPrice = price;
            closePrice = price;
            secondCounter = 0;
        }
    }

    // Now start the live simulation
    secondCounter = 0; // Reset the counter for the live simulation

    while (!stopFlag)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Generate a new price point
        double randStdNormal = normDist(gen);
        double changePercent = volatility * sqrt(dt) * randStdNormal / 100.0; // Convert volatility to a percentage

        // Constrain changePercent to prevent underflow/overflow
        const double maxChangePercent = 0.1; // Maximum 10% change per time step
        if (changePercent > maxChangePercent)
            changePercent = maxChangePercent;
        else if (changePercent < -maxChangePercent)
            changePercent = -maxChangePercent;

        price = price * exp(changePercent);

        // Ensure price does not become zero or negative
        if (price < 0.01)
            price = 0.01;

        // Update price data
        {
            std::lock_guard<std::mutex> dataLock(dataMutex);
            closePrices.push_back(price);
        }

        // Update candle data
        if (secondCounter == 0)
        {
            openPrice = price;
            highPrice = price;
            lowPrice = price;
        }
        else
        {
            if (price > highPrice)
                highPrice = price;
            if (price < lowPrice)
                lowPrice = price;
        }

        closePrice = price;
        secondCounter++;

        if (secondCounter >= candleInterval)
        {
            // Aggregate into a candle
            Candle candle = {openPrice, highPrice, lowPrice, closePrice};
            {
                std::lock_guard<std::mutex> dataLock(dataMutex);
                candles.push_back(candle);
            }

            // Save the candle to disk
            saveCandleToDisk(symbol, candle);

            // Reset for the next interval
            openPrice = price;
            highPrice = price;
            lowPrice = price;
            closePrice = price;
            secondCounter = 0;
        }
    }
}