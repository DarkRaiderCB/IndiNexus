// src/visualization.cpp

#include "utils.h"
#include "visualization.h"

void plotData(const std::string &symbol, const std::vector<Candle> &candles)
{
    std::cout << "Implementation through main.cpp" << std::endl;
}

// Function to calculate Moving Average
double calculateMovingAverage(const std::vector<Candle> &candles, int period, int currentIndex)
{
    if (currentIndex + 1 < period)
        return 0.0; // Not enough data

    double sum = 0.0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i)
    {
        sum += candles[i].close;
    }
    return sum / period;
}

// Function to calculate RSI
double calculateRSI(const std::vector<Candle> &candles, int period, int currentIndex)
{
    if (currentIndex + 1 < period + 1)
        return 50.0; // Neutral RSI when not enough data

    double gain = 0.0;
    double loss = 0.0;

    for (int i = currentIndex - period + 1; i <= currentIndex; ++i)
    {
        double change = candles[i].close - candles[i - 1].close;
        if (change >= 0)
            gain += change;
        else
            loss -= change; // Loss is positive
    }

    if (gain + loss == 0)
        return 50.0; // Prevent division by zero

    double RS = gain / loss;
    double RSI = 100 - (100 / (1 + RS));

    return RSI;
}