#include "client_service.hpp"

#include <stdexcept>

/* TODO: I have to understand how to handle exception and
 * where to put exceptions in C++
 *
 * Another idea could be to create specific domain errors and
 * use them, e.g. functions like addClient return CRM_error_t
 * type and it will be checked but I don't know, maybe it will
 * be addressed later.
 *
 * */

namespace insura::service {

bool ClientService::isEmailUnique(const std::string& email) {
  return !repo_.findByEmail(email).has_value();
}

void ClientService::addClient(const domain::ClientData& client_data) {
  if (!isEmailUnique(client_data.email)) {
    throw std::invalid_argument("Error: Email already used!");
  }

  auto status =
      client_data.lead_status.value_or(domain::Client::ClientStatus::NEW);

  domain::Client client(client_data.first_name, client_data.last_name,
                        client_data.email, client_data.phone,
                        client_data.job_title, client_data.company,
                        client_data.address, client_data.city,
                        client_data.postal_code, status, client_data.notes);

  repo_.insertClient(client);
}

void ClientService::deleteClient(const std::string& uuid) {
  if (!repo_.findByUuid(uuid).has_value())
    throw std::invalid_argument("Error: No contact found");

  repo_.removeClient(uuid);
  /* I should also remove interaction and policies but I haven't
   * implemented them yet */
}

void ClientService::editClient(const std::string& uuid,
                               const domain::ClientData& new_client_data) {
  auto client = repo_.findByUuid(uuid);

  if (!client) {
    throw std::invalid_argument("Error: No contact found");
  }

  if (new_client_data.phone.has_value()) {
    client->setPhone(new_client_data.phone.value());
  }

  if (new_client_data.job_title.has_value()) {
    client->setJobTitle(new_client_data.job_title.value());
  }

  if (new_client_data.company.has_value()) {
    client->setCompany(new_client_data.company.value());
  }

  if (new_client_data.address.has_value()) {
    client->setAddress(new_client_data.address.value());
  }

  if (new_client_data.city.has_value()) {
    client->setCity(new_client_data.city.value());
  }

  if (new_client_data.postal_code.has_value()) {
    client->setPostalCode(new_client_data.postal_code.value());
  }

  if (new_client_data.lead_status.has_value()) {
    client->setStatus(new_client_data.lead_status.value());
  }

  /* Already checked if client is not empty, so using *client is safe */
  repo_.updateClient(*client);
}

std::vector<domain::Client> ClientService::searchClients(
    const std::string& search_term) {
  std::vector<domain::Client> found;

  auto clients = repo_.findAll();

  for (const auto& client : clients) {
    if (client.getFirstName().find(search_term) != std::string::npos ||
        client.getLastName().find(search_term) != std::string::npos) {
      found.push_back(client);
    }
  }

  return found;
}

}  // namespace insura::service
