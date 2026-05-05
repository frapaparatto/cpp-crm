#include "policy_view.hpp"

#include <iomanip>
#include <iostream>

#include "../domain/policy_status.hpp"

namespace insura::cli {

namespace {
constexpr int kColWidth = 20;
const std::string kSeparator(kColWidth * 5, '-');
}  // namespace

void PolicyView::displayAll(const std::vector<domain::Policy>& policies) {
  if (policies.empty()) {
    std::cout << "No policies found.\n";
    return;
  }

  std::cout << std::left << std::setw(kColWidth) << "TYPE"
            << std::setw(kColWidth) << "STATUS" << std::setw(kColWidth)
            << "AMOUNT" << std::setw(kColWidth) << "START DATE"
            << std::setw(kColWidth) << "END DATE" << '\n'
            << kSeparator << '\n';

  for (const auto& p : policies) {
    std::cout << std::left << std::setw(kColWidth)
              << domain::policyTypeToString(p.getPolicyType())
              << std::setw(kColWidth)
              << domain::policyStatusToString(p.getPolicyStatus())
              << std::setw(kColWidth) << p.getPolicyAmount()
              << std::setw(kColWidth) << p.getPolicyStartDate()
              << std::setw(kColWidth)
              << p.getPolicyEndDate().value_or("N/A") << '\n';
  }
}

void PolicyView::displayOne(const domain::Policy& p) {
  auto opt = [](const std::optional<std::string>& v) -> const std::string& {
    static const std::string kNa = "N/A";
    return v.has_value() ? v.value() : kNa;
  };

  std::cout << "UUID:        " << p.getUuid() << '\n'
            << "Client UUID: " << p.getClientUuid() << '\n'
            << "Type:        " << domain::policyTypeToString(p.getPolicyType())
            << '\n'
            << "Status:      "
            << domain::policyStatusToString(p.getPolicyStatus()) << '\n'
            << "Amount:      " << p.getPolicyAmount() << '\n'
            << "Start date:  " << p.getPolicyStartDate() << '\n'
            << "End date:    " << opt(p.getPolicyEndDate()) << '\n'
            << "Notes:       " << opt(p.getPolicyNotes()) << '\n'
            << "Created at:  " << p.getCreatedAt() << '\n'
            << "Updated at:  " << p.getUpdatedAt() << '\n';
}

}  // namespace insura::cli
