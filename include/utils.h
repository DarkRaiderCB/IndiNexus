#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include <atomic>
#include <fstream>
#include <limits>
#include <cstdio>
#include <map>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iomanip>

// Cross-platform includes
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define CLEAR_SCREEN "\033[2J"
#define RESET_CURSOR "\033[H"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEARLINE "\033[K"
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define CLEAR_SCREEN "\033[2J"
#define RESET_CURSOR "\033[H"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEARLINE "\033[K"
#endif

namespace fs = std::filesystem;

// Mutexes for synchronization
extern std::mutex dataMutex;             // Mutex for data synchronization
extern std::mutex consoleMutex;          // Mutex for console output synchronization
extern std::atomic<bool> stopSimulation; // To stop the simulation
extern std::atomic<bool> changeStock;    // To change the stock
extern std::atomic<bool> inputReceived;  // Flag to indicate input has been received

// Map of initial prices and volatility for each asset
extern std::map<std::string, std::pair<double, double>> assetData;

// Structure for Candlestick data
struct Candle
{
    double open;
    double high;
    double low;
    double close;
};

// Function to enable ANSI escape codes in Windows 10 console
#ifdef _WIN32
inline void EnableVirtualTerminalProcessing()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

// Function to move the cursor to (x, y) position
inline void moveCursor(int x, int y)
{
    std::cout << "\033[" << y << ";" << x << "H";
}

// Function to get console size
inline void GetConsoleSize(int &width, int &height)
{
    width = 80;
    height = 24;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
    {
        width = w.ws_col;
        height = w.ws_row;
    }
#endif
}

#endif
