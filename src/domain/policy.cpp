#include "policy.hpp"

#include <stdexcept>

#include "utils.hpp"

namespace insura::domain {

/*
 * Reasoning: client id can't be empty so I don't need to check
 * for it.
 *
 * cli -> select and resolve client -> service find uuid and pass
 * to the constructor
 */
Policy::Policy(std::string client_uuid, PolicyType policy_type,
               std::string start_date, std::optional<std::string> end_date,
               double amount, PolicyStatus policy_status,
               std::optional<std::string> notes)
    : client_uuid_(std::move(client_uuid))

{
  if (start_date.empty()) throw std::runtime_error("Start date can't be empty");
  if (!utils::date::isValidDate(start_date))
    throw std::runtime_error("Invalid date");

  if (end_date && !utils::date::isValidDate(end_date.value()))
    throw std::runtime_error("Invalid date");

  uuid_ = utils::generateUuid();
  policy_type_ = policy_type;
  start_date_ = std::move(start_date);
  /* I think the check end > start should be handled at cli/service level*/
  end_date_ = std::move(end_date);
  amount_ = amount;
  policy_status_ = policy_status;
  notes_ = std::move(notes);
  created_at_ = utils::currentTimestamp();
}

}  // namespace insura::domain
