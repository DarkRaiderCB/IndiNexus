// src/authentication.cpp

#include "utils.h"
#include "authentication.h"

// Implement functions

// Function to encrypt a string
std::string encrypt(const std::string &text, int shift)
{
    std::string encryptedText;
    for (char c : text)
    {
        size_t pos = CHARSET.find(c);
        if (pos != std::string::npos)
        {
            encryptedText += CHARSET[(pos + shift) % CHARSET_SIZE];
        }
        else
        {
            encryptedText += c; // Keep the character as-is if it's not in CHARSET
        }
    }
    return encryptedText;
}

// Function to decrypt a string
std::string decrypt(const std::string &text, int shift)
{
    std::string decryptedText;
    for (char c : text)
    {
        size_t pos = CHARSET.find(c);
        if (pos != std::string::npos)
        {
            decryptedText += CHARSET[(pos + CHARSET_SIZE - shift) % CHARSET_SIZE];
        }
        else
        {
            decryptedText += c; // Keep the character as-is if it's not in CHARSET
        }
    }
    return decryptedText;
}

// Function to validate full name (no special characters)
bool validateFullName(const std::string &fullName)
{
    return std::all_of(fullName.begin(), fullName.end(), [](char c)
                       { return std::isalpha(c) || std::isspace(c); });
}

// Function to validate username (only letters, numbers, and underscores)
bool validateUsername(const std::string &username)
{
    return std::all_of(username.begin(), username.end(), [](char c)
                       { return std::isalnum(c) || c == '_'; });
}

// Function to get hidden input (password) without echoing characters
std::string getHiddenInput()
{
    std::string input;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') // '_getch()' gets the character without echoing it
    {
        if (ch == '\b') // Backspace handling
        {
            if (!input.empty())
            {
                input.pop_back();
                std::cout << "\b \b"; // Erase the last character from console
            }
        }
        else
        {
            input.push_back(ch);
            std::cout << '*'; // Masking with '*'
        }
    }
#else
    // Linux/Unix password masking
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // Save old terminal attributes
    newt = oldt;
    newt.c_lflag &= ~ECHO;                   // Disable echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set new terminal attributes

    std::getline(std::cin, input); // Get hidden input

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old terminal attributes
#endif

    std::cout << std::endl; // Move to the next line after password input
    return input;
}

// Function to signup a new user
bool signup(User &user)
{
    std::cout << "Enter your full name (letters and spaces only): ";
    std::getline(std::cin >> std::ws, user.fullName);

    if (!validateFullName(user.fullName))
    {
        std::cout << "Invalid full name. Only letters and spaces are allowed." << std::endl;
        return false;
    }

    std::cout << "Enter your username (letters, numbers, underscores only): ";
    std::cin >> user.username;

    // Clear the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.sync();

    if (!validateUsername(user.username))
    {
        std::cout << "Invalid username. Only letters, numbers, and underscores are allowed." << std::endl;
        return false;
    }

    // Check if user already exists
    if (fs::exists("data/users/" + user.username + ".txt"))
    {
        std::cout << "Username already exists. Please choose a different one." << std::endl;
        return false;
    }

    std::cout << "Enter your password: ";
    std::string password = getHiddenInput();

    std::cout << "Retype your password: ";
    std::string retypePassword = getHiddenInput();

    if (password != retypePassword)
    {
        std::cout << "Passwords do not match. Please try again." << std::endl;
        return false;
    }

    // Store the password (You might want to hash it in a real application)
    user.password = password;

    // Get initial demo money
    try
    {
        std::cout << "Enter starting demo money (INR): ";
        if (!(std::cin >> user.demoMoney) || user.demoMoney <= 0)
        {
            throw std::invalid_argument("Invalid amount entered for demo money.");
        }
        user.initialDemoMoney = user.demoMoney;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error during signup: " << e.what() << std::endl;
        return false;
    }

    // Create user's data file
    fs::create_directories("data/users");
    user.saveUserData();

    return true;
}

// Function to login an existing user
bool login(User &user)
{
    std::cout << "Enter your username: ";
    std::cin >> user.username;

    // Clear the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.sync();

    // Check if user exists
    if (!fs::exists("data/users/" + user.username + ".txt"))
    {
        std::cout << "User not found. Please sign up first." << std::endl;
        return false;
    }

    std::string inputPassword;

    std::cout << "Enter your password: ";
    inputPassword = getHiddenInput();

    // Load user data
    if (user.loadUserData())
    {
        // Verify the password
        if (inputPassword != user.password)
        {
            std::cout << "Incorrect password." << std::endl;
            return false;
        }

        std::cout << "Welcome back, " << user.fullName << "!" << std::endl;

        // Option to add more money to the existing balance
        char addMoneyOption;
        std::cout << "Do you want to add more money to your wallet? (y/n): ";
        std::cin >> addMoneyOption;

        if (addMoneyOption == 'y' || addMoneyOption == 'Y')
        {
            double extraMoney;
            std::cout << "Enter amount to add: ";
            if (!(std::cin >> extraMoney) || extraMoney <= 0)
            {
                std::cout << "Invalid amount entered. Skipping addition." << std::endl;
            }
            else
            {
                user.demoMoney += extraMoney;
                user.initialDemoMoney += extraMoney; // Update initial demo money as well
            }
        }
        return true;
    }
    return false;
}