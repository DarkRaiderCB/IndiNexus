# IndiNexus - Indian Stock Market Trading Simulator

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#)
[![Build](https://img.shields.io/badge/build-Make-orange.svg)](#building-the-project)

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Technical Architecture](#technical-architecture)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building the Project](#building-the-project)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Trading Mechanics](#trading-mechanics)
- [Data Visualization](#data-visualization)
- [Technical Indicators](#technical-indicators)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Citation](#citation)
- [Educational Value](#educational-value)
- [Project Statistics](#project-statistics)

## Overview

IndiNexus is a comprehensive C++ stock market trading simulator designed to provide realistic trading experience for learning Indian equity markets. This educational platform combines real-time price simulation, technical analysis, portfolio management, and interactive visualization to create an immersive trading environment.

The simulator uses Monte Carlo–style stochastic modeling with Geometric Brownian Motion to generate realistic stock price movements, mimicking actual market movements and trends. Users can practice trading strategies without financial risk while learning technical analysis concepts.

## Features

### User Authentication & Security

- **Secure User Registration**: Complete signup process with input validation
- **Encrypted Password Storage**: A mock/simple encryption algorithm for password security
- **Session Management**: Persistent user sessions with data encryption
- **Input Validation**: Comprehensive validation for usernames, passwords, and personal data

### Real-Time Market Simulation

- **Multi-Asset Support**: 7 predefined Indian stocks with unique characteristics
  - RELYCORP (₹2,600.00, 1.00% volatility)
  - TECHSOL (₹4,200.00, 1.50% volatility)
  - INFOWAVE (₹1,800.00, 1.00% volatility)
  - NDFBANK (₹1,600.00, 0.50% volatility)
  - FMCGUNION (₹2,800.00, 0.80% volatility)
  - METALWORKS (₹160.00, 1.20% volatility)
  - SAFEBANK (₹770.00, 0.60% volatility)
- **Realistic Price Movement**: Geometric Brownian Motion with symbol-specific volatility
- **Candlestick Data Generation**: 10-second interval OHLC candles for technical analysis
- **Multi-threaded Simulation**: Concurrent price updates for all assets

### Trading Operations

- **Market Orders**: Instant buy/sell at current market price
- **Limit Orders**: Set price thresholds for automated execution
- **Portfolio Management**: Real-time position tracking and P&L calculation
- **Transaction History**: Complete audit trail of all trading activities
- **Order Management**: View, and cancel pending limit orders

### Advanced Trading Features

- **Broker Fee Calculation**: Realistic 0.05% fee with ₹20 maximum cap
- **Risk Management**: Sufficient funds and holdings validation
- **Average Price Tracking**: Cost basis calculation for holdings
- **Demo Trading Account**: Virtual starting balance

### Data Visualization & Analysis

- **Real-time Charts**: Live candlestick charts using Gnuplot integration
- **Technical Indicators**:
  - Simple Moving Average (SMA)
  - Relative Strength Index (RSI)
  - Price trend analysis
- **Interactive UI**: Console-based interface with cursor positioning
- **Cross-platform Display**: Windows and Linux terminal compatibility

### Data Persistence

- **User Data Storage**: Encrypted user profiles and trading history
- **Market Data Backup**: Historical price and candle data preservation
- **Session Recovery**: Resume trading sessions with preserved portfolio state
- **File System Management**: Organized data storage with filesystem namespace

## Technical Architecture

### Core Components

1. **Authentication Module** (`authentication.h/cpp`)

   - User registration and login system
   - Password encryption using custom cipher
   - Input validation and security checks

2. **Data Management** (`data_management.h/cpp`)

   - User profile and portfolio structures
   - Transaction recording and retrieval
   - Holdings and order management

3. **Trading Engine** (`trading.h/cpp`)

   - Order execution logic
   - Portfolio calculations
   - Risk management validation

4. **Market Simulation** (`simulations.h/cpp`)

   - Real-time price generation
   - Candlestick data aggregation
   - Multi-threaded market updates

5. **Visualization System** (`visualization.h/cpp`)

   - Chart plotting with Gnuplot
   - Technical indicator calculations
   - Real-time display updates

6. **Data Persistence** (`data_persistence.h/cpp`)

   - File I/O operations
   - Data serialization/deserialization
   - Backup and recovery systems

7. **User Interface** (`ui.h/cpp`)
   - Console-based interactive interface
   - Cross-platform terminal control
   - Real-time portfolio display

### Design Patterns & Principles

- **Multi-threading**: Concurrent simulation and user interaction
- **Mutex Synchronization**: Thread-safe data access
- **RAII**: Resource management and exception safety
- **Cross-platform Compatibility**: Windows and Linux support
- **Modular Architecture**: Separated concerns and clean interfaces

## Prerequisites

### System Requirements

- **Operating System**: Windows 10+ or Linux (Ubuntu 18.04+)
- **Compiler**: GCC 7.0+ or Visual Studio 2019+ (C++17 support required)
- **Memory**: Minimum 512 MB RAM
- **Storage**: 100 MB available disk space

### Dependencies

- **Standard C++ Libraries**: C++17 STL
- **System Libraries**:
  - Windows: `windows.h`, `conio.h`
  - Linux: `termios.h`, `unistd.h`, `sys/ioctl.h`
- **External Tools**:
  - [Gnuplot](http://www.gnuplot.info/) (for chart visualization)
  - Make (for building)

### Installing Gnuplot

#### Windows

```bash
# Using Chocolatey
choco install gnuplot

# Or download from official website
# http://www.gnuplot.info/download.html
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install gnuplot
```

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/DarkRaiderCB/IndiNexus
cd IndiNexus
```

### 2. Verify Directory Structure

```
IndiNexus/
├── Makefile
├── README.md
├── include/
│   ├── authentication.h
│   ├── data_management.h
│   ├── data_persistence.h
│   ├── simulations.h
│   ├── trading.h
│   ├── ui.h
│   ├── utils.h
│   └── visualization.h
└── src/
    ├── authentication.cpp
    ├── data_management.cpp
    ├── data_persistence.cpp
    ├── main.cpp
    ├── simulations.cpp
    ├── trading.cpp
    ├── ui.cpp
    └── visualization.cpp
```

## Building the Project

### Using Make (Recommended)

#### Linux/macOS

```bash
# Build the project
make

# Clean build artifacts
make clean

# Build with verbose output
make all
```

#### Windows (with MinGW/MSYS2)

```bash
# Ensure MinGW is in PATH
make

# For Windows-specific build
make TARGET=IndiNexus.exe
```

### Build Output

- **Executable**: `build/IndiNexus` (Linux/macOS) or `build/IndiNexus.exe` (Windows)
- **Data Directory**: `data/` (automatically created during runtime)

## Usage

### Starting the Application

```bash
# Linux/macOS
./build/IndiNexus

# Windows
build\IndiNexus.exe
```

### User Registration

1. Select option `1` for signup
2. Enter personal information:
   - Full Name (alphabets and spaces only)
   - Username (alphanumeric and underscore)
   - Password (masked input)
3. Complete registration and proceed to login

### Trading Interface

#### Main Menu Commands

- **Stock Selection**: Enter stock symbol (e.g., `RELYCORP`, `TECHSOL`)
- **View Transactions**: Type `transactions`
- **Exit Application**: Type `exit`

#### Trading Commands

| Command              | Description               | Usage                                         |
| -------------------- | ------------------------- | --------------------------------------------- |
| `buy`                | Market buy order          | `buy` → Enter quantity                        |
| `sell`               | Market sell order         | `sell` → Enter quantity                       |
| `limit_buy`          | Limit buy order           | `limit_buy` → Enter quantity and limit price  |
| `limit_sell`         | Limit sell order          | `limit_sell` → Enter quantity and limit price |
| `cancel_limit_order` | Cancel pending order      | Follow interactive prompts                    |
| `help`               | Show command help         | `help`                                        |
| `return_main_menu`   | Return to stock selection | `return_main_menu`                            |
| `exit`               | Exit application          | `exit`                                        |

## Project Structure

```
IndiNexus/
├── Makefile
├── README.md
├── include/
│   ├── authentication.h
│   ├── data_management.h
│   ├── data_persistence.h
│   ├── simulations.h
│   ├── trading.h
│   ├── ui.h
│   ├── utils.h
│   └── visualization.h
└── src/
    ├── authentication.cpp
    ├── data_management.cpp
    ├── data_persistence.cpp
    ├── main.cpp
    ├── simulations.cpp
    ├── trading.cpp
    ├── ui.cpp
    └── visualization.cpp
```

## Trading Mechanics

### Order Types

- Market Orders
- Limit Orders

### Fee Structure

```cpp
double brokerFee = min(transactionValue * 0.0005, 20.0);
// 0.05% of transaction value, capped at ₹20
```

### Portfolio Calculations

- **Average Price**: Weighted average of all purchases
- **Unrealized P&L**: (Current Price - Average Price) × Quantity
- **Realized P&L**: Calculated on sales based on FIFO method

### Risk Management

- **Buying Power Check**: Ensures sufficient demo money
- **Holdings Validation**: Verifies adequate shares for selling
- **Price Bounds**: Prevents extreme price movements

## Data Visualization

### Chart Features

- **Real-time Updates**: 1-second refresh rate
- **Candlestick Display**: OHLC data with 10-second (mock, can set to realistic values too like 4 hrs and so) intervals
- **Technical Overlays**: Moving averages and RSI indicators

### Technical Indicators

#### Simple Moving Average (SMA)

```cpp
double sma = calculateMovingAverage(candles, period, currentIndex);
// Calculates average price over specified period
```

#### Relative Strength Index (RSI)

```cpp
double rsi = calculateRSI(candles, 14, currentIndex);
// Momentum oscillator (0-100 scale)
// >70: Overbought, <30: Oversold
```

#### Candlestick Patterns

- Bullish Engulfing: A larger green candle engulfs a smaller red candle.
- Bearish Engulfing: A larger red candle engulfs a smaller green candle.
- Doji: Open and close prices are nearly equal, indicating indecision.
- Hammer: A candle with a small body and long lower wick, signaling potential reversal.
- Harami: A smaller candle is contained within the range of the previous larger candle.

and many more...

### Input Validation

- **Username**: Alphanumeric characters and underscores only
- **Full Name**: Alphabetic characters and spaces only
- **Password**: Hidden input with masking
- **Numeric Inputs**: Range and type validation

### Data Protection

- **File Encryption**: User data stored with encryption
- **Session Security**: Authenticated access to trading functions
- **Input Sanitization**: Prevention of injection attacks

## Configuration

### Asset Configuration

Modify `assetData` in `main.cpp` to add/edit stocks:

```cpp
std::map<std::string, std::pair<double, double>> assetData = {
    {"SYMBOL", {initialPrice, volatilityPercentage}},
    // Add new stocks here
};
```

### Simulation Parameters

```cpp
// In simulations.cpp
const int candleInterval = 10;     // Seconds per candle
const int preLoadCandles = 50;     // Historical data points
const double maxChangePercent = 0.1; // Maximum price change per step
```

### Fee Structure

```cpp
// In trading.cpp
double feePercentage = transactionValue * 0.0005; // 0.05%
return std::min(feePercentage, 20.0);             // ₹20 maximum
```

## Troubleshooting

### Common Issues

#### Build Errors

**Error**: `fatal error: 'filesystem' file not found`

```bash
# Solution: Ensure C++17 support
g++ -std=c++17 -lstdc++fs src/*.cpp -Iinclude -o build/IndiNexus
```

**Error**: `undefined reference to pthread_create`

```bash
# Solution: Link pthread library
g++ -pthread src/*.cpp -Iinclude -o build/IndiNexus
```

#### Runtime Issues

**Issue**: Gnuplot not found

```bash
# Check installation
gnuplot --version

# Install if missing
sudo apt install gnuplot  # Linux
choco install gnuplot     # Windows
```

**Issue**: Permission denied on data files

```bash
# Fix permissions
chmod 755 data/
chmod 644 data/*
```

**Issue**: Terminal display problems

```bash
# Reset terminal
reset
# Or set TERM environment variable
export TERM=xterm-256color
```

#### Performance Issues

**Issue**: Slow chart updates

- Reduce data points in visualization
- Increase update interval
- Check system resources

**Issue**: High CPU usage

- Optimize simulation thread sleep intervals
- Reduce concurrent asset simulations
- Profile with `gprof` or `valgrind`

### Debug Mode

```bash
# Compile with debug symbols
make CXXFLAGS="-std=c++17 -Wall -g -DDEBUG"

# Run with GDB
gdb ./build/IndiNexus
```

### Log Files

- User data: `data/users/<username>.dat`
- Market data: `data/market_data/<symbol>/`
- Error logs: Check console output

## Contributing

### Development Setup

1. Fork the repository
2. Create feature branch: `git checkout -b feature-name`
3. Follow coding standards (see below)
4. Add tests for new functionality
5. Submit pull request

### Coding Standards

- **Style**: Follow Google C++ Style Guide
- **Naming**: CamelCase for classes, snake_case for functions
- **Documentation**: Doxygen-style comments
- **Error Handling**: Use exceptions appropriately

### Adding New Features

#### New Stock Symbols

1. Add to `assetData` map in `main.cpp`
2. Update validation in trading functions
3. Test with various volatility settings

#### New Order Types

1. Extend `Order` struct in `data_management.h`
2. Add execution logic in `trading.cpp`
3. Update UI prompts in `ui.cpp`

#### New Technical Indicators

1. Add calculation function to `visualization.h/cpp`
2. Integrate with plotting system
3. Add configuration parameters

## License

This project is licensed under a **custom, non-commercial license**.  
You may use, modify, and share the software for **personal, educational, or research purposes only**.  
Commercial use, patent rights, or misrepresentation of authorship are strictly prohibited.  
Proper attribution is required, and explicit permission must be obtained for use in research publications or integration into other projects.  
Contributors will be credited for their contributions in any public use.

See the [LICENSE](LICENSE) file for complete terms.

---

## Citation

If you use this software in research, academic work, or publications, please cite it as follows:

**APA Style:**

> Mishra, S., & Project Contributors. (2025). _INDINEXUS: An Indian Equity Market Trading Simulator_ [Computer software]. Available at https://github.com/DarkRaiderCB/IndiNexus

**BibTeX:**

```bibtex
@software{indinexus2025,
  author = {Mishra, Sanyog and Project Contributors},
  title = {INDINEXUS: An Indian Equity Market Trading Simulator},
  year = {2025},
  url = {https://github.com/DarkRaiderCB/IndiNexus},
  note = {Non-commercial use only. Proper attribution and acknowledgements required.}
}
```

For full citation metadata, see the [CITATION.cff](CITATION.cff) file.

## Educational Value

This project serves as an excellent learning resource for:

- **C++ Programming**: Advanced concepts like multi-threading, file I/O, OOP
- **Financial Markets**: Understanding of trading mechanics and market behavior
- **Software Architecture**: Modular design and cross-platform development
- **Data Structures**: Efficient data management and algorithms
- **System Programming**: Terminal control and process management

## Project Statistics

- **Lines of Code**: ~3,000+ (excluding comments and whitespace)
- **Files**: 16 source/header files
- **Classes/Structures**: 8 main data structures
- **Functions**: 50+ implemented functions
- **Supported Platforms**: Windows, Linux, macOS
- **External Dependencies**: Gnuplot

---
