#pragma once
#include <string>
#include <string_view>

#include "client.hpp"

namespace insura::domain {

std::string statusToString(Client::ClientStatus status);
Client::ClientStatus statusFromString(std::string_view str);

}  // namespace insura::domain
