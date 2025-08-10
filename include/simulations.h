#ifndef SIMULATIONS_H
#define SIMULATIONS_H

#include "utils.h"

// Function declarations
void simulateSymbolPrice(const std::string &symbol, std::vector<double> &closePrices, std::vector<Candle> &candles, std::atomic<bool> &stopFlag);

#endif // SIMULATIONS_H