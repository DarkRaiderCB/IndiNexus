// src/data_persistence.cpp

#include "utils.h"
#include "data_persistence.h"

// Define the variables
std::map<std::string, std::vector<double>> closePricesMap;
std::map<std::string, std::vector<Candle>> candlesMap;
std::map<std::string, std::atomic<bool>> stopFlags;

// Function to save stock data to disk
void saveStockData(const std::map<std::string, std::vector<double>> &closePricesMap,
                   const std::map<std::string, std::vector<Candle>> &candlesMap)
{
    std::lock_guard<std::mutex> dataLock(dataMutex); // Ensure thread safety

    fs::create_directories("data/stock_data"); // Ensure the directory exists

    // Save closePricesMap
    for (const auto &pair : closePricesMap)
    {
        const std::string &symbol = pair.first;
        const std::vector<double> &prices = pair.second;

        std::ofstream outFile("data/stock_data/" + symbol + "_closePrices.dat", std::ios::binary);
        if (outFile.is_open())
        {
            size_t size = prices.size();
            outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));
            outFile.write(reinterpret_cast<const char *>(prices.data()), size * sizeof(double));
            outFile.close();
        }
    }

    // Save candlesMap
    for (const auto &pair : candlesMap)
    {
        const std::string &symbol = pair.first;
        const std::vector<Candle> &candles = pair.second;

        std::ofstream outFile("data/stock_data/" + symbol + "_candles.dat", std::ios::binary);
        if (outFile.is_open())
        {
            size_t size = candles.size();
            outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));
            outFile.write(reinterpret_cast<const char *>(candles.data()), size * sizeof(Candle));
            outFile.close();
        }
    }
}

// Function to load stock data from disk
void loadStockData(std::map<std::string, std::vector<double>> &closePricesMap,
                   std::map<std::string, std::vector<Candle>> &candlesMap)
{
    std::lock_guard<std::mutex> dataLock(dataMutex); // Ensure thread safety

    // Load closePricesMap
    for (const auto &pair : assetData)
    {
        const std::string &symbol = pair.first;

        std::ifstream inFile("data/stock_data/" + symbol + "_closePrices.dat", std::ios::binary);
        if (inFile.is_open())
        {
            size_t size = 0;
            inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
            std::vector<double> prices(size);
            inFile.read(reinterpret_cast<char *>(prices.data()), size * sizeof(double));
            closePricesMap[symbol] = prices;
            inFile.close();
        }
    }

    // Load candlesMap
    for (const auto &pair : assetData)
    {
        const std::string &symbol = pair.first;

        std::ifstream inFile("data/stock_data/" + symbol + "_candles.dat", std::ios::binary);
        if (inFile.is_open())
        {
            size_t size = 0;
            inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
            std::vector<Candle> candles(size);
            inFile.read(reinterpret_cast<char *>(candles.data()), size * sizeof(Candle));
            candlesMap[symbol] = candles;
            inFile.close();
        }
    }
}

// Function to save a candle to disk
void saveCandleToDisk(const std::string &symbol, const Candle &candle)
{
    fs::create_directories("data/stock_data/" + symbol); // Ensure the directory exists
    std::ofstream outFile("data/stock_data/" + symbol + "/candles_history.dat", std::ios::binary | std::ios::app);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char *>(&candle), sizeof(candle));
        outFile.close();
    }
    else
    {
        std::cerr << "Error: Could not write candle data for symbol " << symbol << std::endl;
    }
}