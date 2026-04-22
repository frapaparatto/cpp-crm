#pragma once
#include <string>

namespace insura::domain::utils {
std::string generateUuid();
std::string currentTimestamp();
bool isValidEmail(std::string_view email);
bool isDigitsOnly(std::string_view str);
bool isValidPhone(std::string_view phone);
}  // namespace insura::domain::utils
