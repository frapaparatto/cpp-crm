#include "./csv_policy_repository.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

#include "../domain/policy_status.hpp"
#include "../domain/utils.hpp"
#include "file_handle.hpp"

namespace insura::data {
CsvPolicyRepository::CsvPolicyRepository(std::string filepath)
    : filepath_(std::move(filepath)) {}

void CsvPolicyRepository::insertPolicy(domain::Policy policy) {
  policies_.push_back(std::move(policy));
  dirty_ = true;
}

void CsvPolicyRepository::removePolicy(std::string_view uuid) {
  auto it = std::remove_if(
      policies_.begin(), policies_.end(),
      [uuid](const domain::Policy& p) { return p.getUuid() == uuid; });

  if (it != policies_.end()) {
    policies_.erase(it, policies_.end());
    dirty_ = true;
  }
}

std::optional<domain::Policy> CsvPolicyRepository::findByUuid(
    std::string_view uuid) const {
  auto it = std::find_if(policies_.begin(), policies_.end(),
                         [uuid](const domain::Policy& p) {
                           return p.getUuid() == uuid;
                         });

  if (it == policies_.end()) return std::nullopt;
  return *it;
}

std::vector<domain::Policy> CsvPolicyRepository::findByClientUuid(
    std::string_view client_uuid) const {
  std::vector<domain::Policy> found;

  std::copy_if(policies_.begin(), policies_.end(), std::back_inserter(found),
               [client_uuid](const domain::Policy& p) {
                 return p.getClientUuid() == client_uuid;
               });

  return found;
}

const std::vector<domain::Policy>& CsvPolicyRepository::findAll() const {
  return policies_;
}

void CsvPolicyRepository::updatePolicy(domain::Policy updated) {
  auto it = std::find_if(policies_.begin(), policies_.end(),
                         [&updated](const domain::Policy& p) {
                           return p.getUuid() == updated.getUuid();
                         });

  if (it != policies_.end()) {
    *it = std::move(updated);
    dirty_ = true;
  }
}

std::string CsvPolicyRepository::serialize(const domain::Policy& p) const {
  std::ostringstream ss;
  ss << p.getUuid() << ",";
  ss << p.getClientUuid() << ",";
  ss << domain::policyTypeToString(p.getPolicyType()) << ",";
  ss << p.getPolicyStartDate() << ",";
  ss << p.getPolicyEndDate().value_or("") << ",";
  ss << std::to_string(p.getPolicyAmount()) << ",";
  ss << domain::policyStatusToString(p.getPolicyStatus()) << ",";
  ss << p.getPolicyNotes().value_or("") << ",";
  ss << p.getCreatedAt() << ",";
  ss << p.getUpdatedAt();

  return ss.str();
}

domain::Policy CsvPolicyRepository::deserialize(const std::string& line) const {
  std::stringstream ss(line);
  std::string uuid, client_uuid, policy_type, start_date, end_date, amount,
      policy_status, notes, created_at, updated_at;

  std::getline(ss, uuid, ',');
  std::getline(ss, client_uuid, ',');
  std::getline(ss, policy_type, ',');
  std::getline(ss, start_date, ',');
  std::getline(ss, end_date, ',');
  std::getline(ss, amount, ',');
  std::getline(ss, policy_status, ',');
  std::getline(ss, notes, ',');
  std::getline(ss, created_at, ',');
  std::getline(ss, updated_at, ',');

  return domain::Policy(uuid, client_uuid,
                        domain::policyTypeFromString(policy_type), start_date,
                        utils::stringToOptional(end_date), std::stod(amount),
                        domain::policyStatusFromString(policy_status),
                        utils::stringToOptional(notes), created_at, updated_at);
}

/* TODO: since the new decision record I've made says that
 * I set default names for files, those checks happen before
 * application starts so filepath exists
 */

void CsvPolicyRepository::load() {
  std::vector<domain::Policy> tmp_policies;

  if (std::filesystem::exists(filepath_)) {
    FileHandler in(filepath_, std::ios::in);
    std::string line;

    while (std::getline(in.getStream(), line)) {
      tmp_policies.push_back(deserialize(line));
    }
  } else {
    throw std::runtime_error("Error: File doesn't exists");
  }

  policies_ = std::move(tmp_policies);
}

void CsvPolicyRepository::save() const {
  std::string tmp = filepath_ + ".tmp";
  {
    /* Scoped because destructor needs to run */
    FileHandler out(tmp, std::ios::out);
    for (const auto& policy : policies_) {
      out.getStream() << serialize(policy) << '\n';
    }
  }
  std::rename(tmp.c_str(), filepath_.c_str());
}
}  // namespace insura::data
