// src/data_management.cpp

#include "utils.h"
#include "data_management.h"
#include "authentication.h"

void User::saveUserData()
{
    std::ofstream outFile("data/users/" + username + ".txt");
    if (!outFile)
    {
        std::cerr << "Error saving user data." << std::endl;
        return;
    }

    // Save fullName
    outFile << encrypt(fullName, ENCRYPTION_SHIFT) << '\n';

    // Save username
    outFile << encrypt(username, ENCRYPTION_SHIFT) << '\n';

    // Save password
    outFile << encrypt(password, ENCRYPTION_SHIFT) << '\n';

    // Save demoMoney
    outFile << encrypt(std::to_string(demoMoney), ENCRYPTION_SHIFT) << '\n';

    // Save holdings
    outFile << encrypt("Holdings:", ENCRYPTION_SHIFT) << '\n';
    for (const auto &holding : holdings)
    {
        outFile << encrypt(holding.symbol, ENCRYPTION_SHIFT) << ' '
                << encrypt(std::to_string(holding.amount), ENCRYPTION_SHIFT) << ' '
                << encrypt(std::to_string(holding.averagePrice), ENCRYPTION_SHIFT)
                << '\n';
    }

    // Save pendingOrders
    outFile << encrypt("PendingOrders:", ENCRYPTION_SHIFT) << '\n';
    for (const auto &order : pendingOrders)
    {
        outFile << encrypt(order.symbol, ENCRYPTION_SHIFT) << ' '
                << encrypt(order.type, ENCRYPTION_SHIFT) << ' '
                << encrypt(std::to_string(order.amount), ENCRYPTION_SHIFT) << ' '
                << encrypt(std::to_string(order.limitPrice), ENCRYPTION_SHIFT) << '\n';
    }

    // Save transactions
    outFile << encrypt("Transactions:", ENCRYPTION_SHIFT) << '\n';
    for (const auto &transaction : transactions)
    {
        std::ostringstream oss;
        oss << std::get<0>(transaction) << " " << std::get<1>(transaction) << " " << std::get<2>(transaction) << " " << std::get<3>(transaction) << " " << std::get<4>(transaction);
        outFile << encrypt(oss.str(), ENCRYPTION_SHIFT) << '\n';
    }

    outFile.close();
}

// Replace the loadUserData() function with the following code
bool User::loadUserData()
{
    std::ifstream userFile("data/users/" + username + ".txt");
    if (!userFile)
    {
        std::cerr << "Error loading user data." << std::endl;
        return false;
    }

    std::string encryptedLine;

    // Read fullName
    std::getline(userFile, encryptedLine);
    fullName = decrypt(encryptedLine, ENCRYPTION_SHIFT);

    // Read username
    std::getline(userFile, encryptedLine);
    username = decrypt(encryptedLine, ENCRYPTION_SHIFT);

    // Read password
    std::getline(userFile, encryptedLine);
    password = decrypt(encryptedLine, ENCRYPTION_SHIFT);

    // Read demoMoney
    std::getline(userFile, encryptedLine);
    demoMoney = std::stod(decrypt(encryptedLine, ENCRYPTION_SHIFT));

    // Read Holdings
    std::getline(userFile, encryptedLine);
    std::string decryptedLine = decrypt(encryptedLine, ENCRYPTION_SHIFT);
    if (decryptedLine != "Holdings:")
    {
        throw std::runtime_error("Invalid data format: Holdings section missing.");
    }

    holdings.clear();
    while (std::getline(userFile, encryptedLine))
    {
        decryptedLine = decrypt(encryptedLine, ENCRYPTION_SHIFT);
        if (decryptedLine == "PendingOrders:")
        {
            break;
        }
        std::istringstream iss(decryptedLine);
        Holding holding;
        if (iss >> holding.symbol >> holding.amount >> holding.averagePrice)
        {
            holdings.push_back(holding);
        }
        else
        {
            std::cerr << "Error parsing holding: " << decryptedLine << std::endl;
        }
    }

    // Read PendingOrders
    pendingOrders.clear();
    while (std::getline(userFile, encryptedLine))
    {
        decryptedLine = decrypt(encryptedLine, ENCRYPTION_SHIFT);
        if (decryptedLine == "Transactions:")
        {
            break;
        }
        std::istringstream iss(decryptedLine);
        Order order;
        if (iss >> order.symbol >> order.type >> order.amount >> order.limitPrice)
        {
            pendingOrders.push_back(order);
        }
        else
        {
            std::cerr << "Error parsing order: " << decryptedLine << std::endl;
        }
    }

    // Read Transactions
    transactions.clear();
    while (std::getline(userFile, encryptedLine))
    {
        decryptedLine = decrypt(encryptedLine, ENCRYPTION_SHIFT);
        std::istringstream iss(decryptedLine);
        std::string symbol, type;
        double amount, price, brokerFee;
        if (iss >> symbol >> amount >> price >> type >> brokerFee)
        {
            transactions.emplace_back(symbol, amount, price, type, brokerFee);
        }
        else
        {
            std::cerr << "Error parsing transaction: " << decryptedLine << std::endl;
        }
    }

    userFile.close();
    return true;
}