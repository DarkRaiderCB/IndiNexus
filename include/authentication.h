#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "utils.h"
#include "data_management.h"

// Encryption constants
const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_!@#$%^&*";
const int CHARSET_SIZE = CHARSET.size();
const int ENCRYPTION_SHIFT = 5; // You can choose any shift value

// Function declarations
std::string encrypt(const std::string &text, int shift);
std::string decrypt(const std::string &text, int shift);
bool validateFullName(const std::string &fullName);
bool validateUsername(const std::string &username);
std::string getHiddenInput();
bool signup(User &user);
bool login(User &user);

#endif // AUTHENTICATION_H
