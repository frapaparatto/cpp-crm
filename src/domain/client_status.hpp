#pragma once
#include <string>

#include "client.hpp"

namespace insura::domain {

std::string statusToString(Client::ClientStatus status);
Client::ClientStatus statusFromString(const std::string& str);

}  // namespace insura::domain
