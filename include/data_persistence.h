#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include "utils.h"

// Declaration of external variables
extern std::map<std::string, std::vector<double>> closePricesMap;
extern std::map<std::string, std::vector<Candle>> candlesMap;
extern std::map<std::string, std::atomic<bool>> stopFlags;

// Function declarations
void saveStockData(const std::map<std::string, std::vector<double>> &closePricesMap, const std::map<std::string, std::vector<Candle>> &candlesMap);

void loadStockData(std::map<std::string, std::vector<double>> &closePricesMap, std::map<std::string, std::vector<Candle>> &candlesMap);

void saveCandleToDisk(const std::string &symbol, const Candle &candle);

#endif // DATA_PERSISTENCE_H