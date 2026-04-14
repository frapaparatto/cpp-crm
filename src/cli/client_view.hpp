#pragma once
#include <vector>

#include "client.hpp"

namespace insura::cli {

class ClientView {
 public:
  static void displayAll(const std::vector<domain::Client>& clients);
  static void displayOne(const domain::Client& client);
};

}  // namespace insura::cli
