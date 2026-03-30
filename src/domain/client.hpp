#pragma once
#include <string>

/*
 * Notes:
 * - Google C++ Style Guide:
 *   - Use PascalCase for methods and functions, I've decided to use camelCase
 * - No destructor: I've not allocated memory, default destructor is enough
 * - I should decide if keeping date as string type or change it
 */

namespace insura::domain {

class Client {
 public:
  enum class LeadStatus {
    NEW,
    CONTACTED,
    IN_PROGRESS,
    OPEN_DEAL,
    ATTEMPTED_TO_CONTACT,
    CLOSED_WON,
    CLOSED_LOST,
  };

  Client(std::string first_name, std::string last_name, std::string email);

  const std::string& getUuid() const;
  const std::string& getEmail() const;
  const std::string& getFirstName() const;
  const std::string& getLastName() const;
  void setPhone(const std::string& phone);
  // ... other accessors

 private:
  std::string uuid_;
  std::string first_name_;
  std::string last_name_;
  std::string email_;
  std::string phone_;
  std::string address_;
  std::string city_;
  std::string postal_code_;
  std::string job_title_;
  std::string company_;
  std::string notes_;
  LeadStatus lead_status_;
  std::string created_at_;
  std::string updated_at_;
};

}  // namespace insura::domain
