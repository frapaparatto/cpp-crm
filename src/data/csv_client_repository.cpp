#include "csv_client_repository.hpp"

#include <filesystem>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../domain/client_status.hpp"
#include "file_handle.hpp"

/* TODO: I have to remember to do defensive checks for operations
 *
 * But I think is not its responsibility?
 * I am not sure becasue the dirty flag is updated here and since it must be
 * updated only if operation succeed, I should make some checks.
 *
 * */

namespace insura::data {

CSVClientRepository::CSVClientRepository(const std::string& filepath)
    : filepath_(filepath) {}

void CSVClientRepository::insertClient(const domain::Client& client) {
  clients_.push_back(client);
  dirty_ = true;
}

void CSVClientRepository::removeClient(const std::string& uuid) {
  auto it = std::remove_if(clients_.begin(), clients_.end(),
                           [&uuid](const domain::Client& client) {
                             return client.getUuid() == uuid;
                           });
  if (it != clients_.end()) {
    clients_.erase(it, clients_.end());
    dirty_ = true;
  }
}

std::optional<domain::Client> CSVClientRepository::findByUuid(
    const std::string& uuid) const {
  auto it = std::find_if(
      clients_.begin(), clients_.end(),
      [&uuid](const domain::Client& c) { return c.getUuid() == uuid; });

  if (it == clients_.end()) return std::nullopt;
  return *it;
}

std::optional<domain::Client> CSVClientRepository::findByEmail(
    const std::string& email) const {
  auto it = std::find_if(
      clients_.begin(), clients_.end(),
      [&email](const domain::Client& c) { return c.getEmail() == email; });

  if (it == clients_.end()) return std::nullopt;
  return *it;
}

const std::vector<domain::Client>& CSVClientRepository::findAll() const {
  return clients_;
}

void CSVClientRepository::updateClient(const domain::Client& updated) {
  auto it = std::find_if(clients_.begin(), clients_.end(),
                         [&updated](const domain::Client& c) {
                           return c.getUuid() == updated.getUuid();
                         });

  if (it != clients_.end()) {
    *it = updated;
    dirty_ = true;
  }
}

void CSVClientRepository::load() {
  std::vector<domain::Client> tmp_clients;

  if (std::filesystem::exists(filepath_)) {
    {
      FileHandler in(filepath_, std::ios::in);
      std::string line;

      while (std::getline(in.getStream(), line)) {
        tmp_clients.push_back(deserialize(line));
      }
    }
  } else {
    throw std::runtime_error("Error: File doesn't exist");
  };

  clients_ = std::move(tmp_clients);
}

void CSVClientRepository::save() const {
  std::string tmp = filepath_ + ".tmp";
  {
    FileHandler out(tmp, std::ios::out);
    for (const auto& c : clients_) {
      out.getStream() << serialize(c) << "\n";
    }
  }
  std::rename(tmp.c_str(), filepath_.c_str());  // atomic swap
}

std::string CSVClientRepository::serialize(const domain::Client& c) const {
  std::ostringstream ss;
  ss << c.getUuid() << ",";
  ss << c.getFirstName() << ",";
  ss << c.getLastName() << ",";
  ss << c.getEmail() << ",";

  ss << c.getPhone().value_or("") << ",";
  ss << c.getJobTitle().value_or("") << ",";
  ss << c.getCompany().value_or("") << ",";
  ss << c.getAddress().value_or("") << ",";
  ss << c.getCity().value_or("") << ",";
  ss << c.getPostalCode().value_or("") << ",";

  ss << domain::statusToString(c.getStatus()) << ",";
  ss << c.getNotes().value_or("") << ",";
  ss << c.getCreatedAt() << ",";
  ss << c.getUpdatedAt();

  return ss.str();
}

domain::Client CSVClientRepository::deserialize(const std::string& line) const {
  std::stringstream ss(line);
  std::string uuid, first, last, email, phone, job, company, address, city,
      postal_code, status, notes, created_at, updated_at;

  std::getline(ss, uuid, ',');
  std::getline(ss, first, ',');
  std::getline(ss, last, ',');
  std::getline(ss, email, ',');
  std::getline(ss, phone, ',');
  std::getline(ss, job, ',');
  std::getline(ss, company, ',');
  std::getline(ss, address, ',');
  std::getline(ss, city, ',');
  std::getline(ss, postal_code, ',');
  std::getline(ss, status, ',');
  std::getline(ss, notes, ',');
  std::getline(ss, created_at, ',');
  std::getline(ss, updated_at, ',');

  domain::Client::ClientStatus lead_status = domain::statusFromString(status);

  return domain::Client(uuid, first, last, email, phone, job, company, address,
                        city, postal_code, lead_status, notes, created_at,
                        updated_at);
}

}  // namespace insura::data
