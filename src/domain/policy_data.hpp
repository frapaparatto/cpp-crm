#pragma once

#include <string>

#include "./policy.hpp"

namespace insura::domain {

struct PolicyData {
  /* Remember that client_uuid on edit can't change */
  std::string client_uuid;
  Policy::PolicyType policy_type_;
  std::string start_date;
  std::optional<std::string> end_date;
  double amount;
  std::optional<Policy::PolicyStatus> policy_status;
  std::optional<std::string> notes;
};
}  // namespace insura::domain
