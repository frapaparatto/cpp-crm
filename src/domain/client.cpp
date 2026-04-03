#include "client.hpp"

#include <cctype>
#include <ctime>
#include <regex>
#include <stdexcept>

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

/* TODO: update this function to be more restrictive */
static bool isValidEmail(const std::string& email) {
  const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");

  return std::regex_match(email, pattern);
}

static bool isNumber(const std::string& str) {
  /* Function explanation:
   *  - AND condition: is true if both are true
   *  - first check: the string is not empty, if empty the function returns
   *    true immediately
   *  - then usee the function find if to ensure all elements are digit
   *    - find if returns an iterator to the first element for which the
   *    function returns true
   *    - since I've setted it to return false, if the postal code is correct
   *    it should return the last element that is str.end()
   *    - if the postal code is wrong, it will return something that is not a
   *    digit and the final check will be false */
  return !str.empty() &&
         std::find_if(str.begin(), str.end(), [](unsigned char c) {
           return !std::isdigit(c);
         }) == str.end();
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
  lead_status_ = ClientStatus::NEW;
}

const std::string& Client::getUuid() const {return uuid_;}
const std::string& Client::getFirstName() const { return first_name_; }
const std::string& Client::getLastName() const { return last_name_; }
const std::string& Client::getEmail() const { return email_; }
const std::string& Client::getPhone() const { return phone_; }
const std::string& Client::getAddress() const { return address_; }
const std::string& Client::getCity() const { return city_; }
const std::string& Client::getPostalCode() const { return postal_code_; }
const std::string& Client::getJobTitle() const { return job_title_; }
const std::string& Client::getCompany() const { return company_; }
Client::ClientStatus Client::getStatus() const { return lead_status_; }
const std::string& Client::getUpdatedAt() const { return updated_at_; }
const std::string& Client::createdAt() const { return created_at_; }

// TODO: add the notes getter/setter

/* TODO: here I should add validation for each field but for now I will maintain
 * things simple */
void Client::setPhone(const std::string phone) {
  /* simple validation: check if all char are numbers
   * TODO: add validation based on format like +39... and standardise
   * format especially in the display */
  if (!isNumber(phone)) throw std::invalid_argument("invalid phone number");

  phone_ = std::move(phone);
  updated_at_ = utils::currentTimestamp();
}
void Client::setAddress(const std::string address) {
  address_ = std::move(address);
  updated_at_ = utils::currentTimestamp();
}
void Client::setCity(const std::string city) {
  city_ = std::move(city);
  updated_at_ = utils::currentTimestamp();
}
void Client::setPostalCode(const std::string postal_code) {
  if (!isNumber(postal_code))
    throw std::invalid_argument("invalid postal code");

  postal_code_ = std::move(postal_code);
  updated_at_ = utils::currentTimestamp();
}

/* TODO: evaluate to make the job title an enum or something like that
 * and choose between fixed alternatives in order to create a standardized
 * way to represents jobs
 *
 * Same for companies, choose a fixed pre-set of companies */
void Client::setJobTitle(const std::string job_title) {
  job_title_ = std::move(job_title);
  updated_at_ = utils::currentTimestamp();
}
void Client::setCompany(const std::string company) {
  company_ = std::move(company);
  updated_at_ = utils::currentTimestamp();
}

void Client::setStatus(ClientStatus status) {
  lead_status_ = status;
  updated_at_ = utils::currentTimestamp();
}

}  // namespace insura::domain
