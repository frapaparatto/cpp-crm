#pragma once
#include <string>
namespace insura::domain::strops {

std::string lower(std::string s);
std::string capitalize(std::string s);
std::string trim(std::string s);
bool contains(std::string_view text, std::string_view query);

}  // namespace insura::domain::strops
