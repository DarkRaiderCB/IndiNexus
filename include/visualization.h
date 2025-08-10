#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "utils.h"
#include "simulations.h"

// Function declarations
void plotData(const std::string &symbol, const std::vector<Candle> &candles);
double calculateMovingAverage(const std::vector<Candle> &candles, int period, int currentIndex);
double calculateRSI(const std::vector<Candle> &candles, int period, int currentIndex);

#endif // VISUALIZATION_H
