#pragma once
#include <string>
#include <string_view>

#include "policy.hpp"

namespace insura::domain {

std::string policyStatusToString(Policy::PolicyStatus status);
Policy::PolicyStatus policyStatusFromString(std::string_view str);

std::string policyTypeToString(Policy::PolicyType type);
Policy::PolicyType policyTypeFromString(std::string_view str);

}  // namespace insura::domain
