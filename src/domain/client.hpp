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
  enum class ClientStatus {
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
  const std::string& getUpdatedAt() const;
  const std::string& createdAt() const;


  void setPhone(const std::string phone);
  void setAddress(const std::string address);
  void setCity(const std::string city);
  void setPostalCode(const std::string postal_code);
  void setJobTitle(const std::string job_title);
  void setCompany(const std::string company);
  void setStatus(ClientStatus status);

  const std::string& getPhone() const;
  const std::string& getAddress() const;
  const std::string& getCity() const;
  const std::string& getPostalCode() const;
  const std::string& getJobTitle() const;
  const std::string& getCompany() const;
  ClientStatus getStatus() const;

  /* TODO: understand how to add note field: I mean, the simplest form is
   * to write and press enter but maybe I want some specialized behaviours */

 private:
  /* evaluate to add birthay date since CRM may wants to know in order to
   * create custom offers for this special day in order to lead the customer
   * make make another contract */
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
  ClientStatus lead_status_;
  std::string created_at_;
  std::string updated_at_;
};

}  // namespace insura::domain
