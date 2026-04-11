#pragma once
#include <optional>
#include <string>

#include "client.hpp"

namespace insura::domain {

struct ClientData {
  std::string first_name;
  std::string last_name;
  std::string email;

  std::optional<std::string> phone;
  std::optional<std::string> job_title;
  std::optional<std::string> company;
  std::optional<std::string> address;
  std::optional<std::string> city;
  std::optional<std::string> postal_code;
  std::optional<std::string> notes;

  std::optional<Client::ClientStatus> lead_status;
};

}  // namespace insura::domain
