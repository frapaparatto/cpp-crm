#pragma once
#include <string>

namespace insura::domain::utils {
std::string generateUuid();
std::string currentTimestamp();
bool isValidEmail(const std::string& email);
bool isDigitsOnly(const std::string& str);
bool isValidPhone(const std::string& phone);
}
