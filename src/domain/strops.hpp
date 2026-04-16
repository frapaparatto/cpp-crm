#pragma once
#include <string>
namespace insura::domain::strops {

std::string lower(std::string s);
std::string capitalize(std::string s);
std::string trim(std::string s);
bool contains(const std::string& text, const std::string& query);

}  // namespace insura::domain::strops
