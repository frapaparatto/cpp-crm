#include "client.hpp"

#include <ctime>
#include <regex>

#include "utils.hpp"

/*
 * Reasoning behind design: I don't want to use exceptions in
 * each part of my code, I want to balance defensive checks and
 * exceptions handling.
 *
 * I mean, I've studied the principle according to which if you
 * expect something, don't use exception. Use them only for things
 * that you don't expect
 *
 * At the same time, I don't know in C++ if this is valid since
 * I've studied it for Python but in Python is easier to manage
 * empty inputs or something like that, in C++ you should handle
 * memory, pointers and stuff like that.
 *
 * This is what I think about exceptions (I will ignore the
 * distinction I've made above):
 * You should handle exceptions differently depending on the layer
 * you are in:
 * - E.g. error during program startup (e.g. setup): terminate
 *   the program early
 * - error during the main operations (user layer): status code
 * - error during mid-operations (e.g. adding a client): handle
 *   it and ensure atomicity
 *
 * Then I have to do the following
 * - understand if the regex could be improved
 * - why std::move for copying attributes
 *
 */

static bool isValidEmail(const std::string& email) {
  const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");

  return std::regex_match(email, pattern);
}

namespace insura::domain {

Client::Client(std::string first_name, std::string last_name,
               std::string email) {

  if (first_name.empty()) {
    throw std::invalid_argument("first name cannot be empty");
  }

  if (last_name.empty()) {
    throw std::invalid_argument("last name cannot be empty");
  }

  if (email.empty()) {
    throw std::invalid_argument("email cannot be empty");
  }

  if (!isValidEmail(email)) {
    throw std::invalid_argument("invalid email");
  }

  uuid_ = utils::generateUuid();
  first_name_ = std::move(first_name);
  last_name_ = std::move(last_name);
  email_ = std::move(email);
  created_at_ = utils::currentTimestamp();
  lead_status_ = LeadStatus::NEW;
}

const std::string& Client::getFirstName() const { return first_name_; }
const std::string& Client::getLastName() const { return last_name_; }
const std::string& Client::getEmail() const { return email_; }

}  // namespace insura::domain
