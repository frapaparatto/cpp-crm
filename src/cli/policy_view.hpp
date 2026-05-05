#pragma once
#include <vector>

#include "../domain/policy.hpp"

namespace insura::cli {

class PolicyView {
 public:
  static void displayAll(const std::vector<domain::Policy>& policies);
  static void displayOne(const domain::Policy& policy);
};

}  // namespace insura::cli
