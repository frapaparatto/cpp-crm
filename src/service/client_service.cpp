#include "client_service.hpp"

#include <stdexcept>

#include "../domain/strops.hpp"

namespace insura::service {

bool ClientService::isEmailUnique(std::string_view email) const {
  return !repo_.findByEmail(email).has_value();
}

/* Inputs in client_data are expected to be normalized by the caller (trimmed,
 * capitalized, email lowercased). No normalization is performed here.
 * If a non-CLI caller (e.g. a future BatchImportService) uses this method,
 * it is responsible for preparing clean data before calling addClient.
 */
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

  repo_.insertClient(std::move(client));
}

void ClientService::deleteClient(std::string_view uuid) {
  if (!repo_.findByUuid(uuid).has_value())
    throw std::invalid_argument("Error: No contact found");

  repo_.removeClient(uuid);
  /* I should also remove interaction and policies but I haven't
   * implemented them yet */
}

void ClientService::editClient(std::string_view uuid,
                               const domain::ClientData& new_client_data) {
  auto client = repo_.findByUuid(uuid);

  if (!client) {
    throw std::invalid_argument("Error: No contact found");
  }

  if (!new_client_data.first_name.empty()) {
    client->setFirstName(new_client_data.first_name);
  }

  if (!new_client_data.last_name.empty()) {
    client->setLastName(new_client_data.last_name);
  }

  if (!new_client_data.email.empty()) {
    if (new_client_data.email != client->getEmail() &&
        !isEmailUnique(new_client_data.email)) {
      throw std::invalid_argument("Error: Email already used!");
    }
    client->setEmail(new_client_data.email);
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

  if (new_client_data.notes.has_value()) {
    client->setNotes(new_client_data.notes.value());
  }

  /*
   * *client is an lvalue (stored inside std::optional). std::move casts it to
   * an rvalue so updateClient move-constructs its by-value parameter instead of
   * copying. One unavoidable copy occurred when findByUuid returned into the
   * optional — everything after is zero-copy.
   */
  repo_.updateClient(std::move(*client));
}

std::vector<domain::Client> ClientService::searchClients(
    std::string_view search_term) const {
  std::vector<domain::Client> found;

  auto clients = repo_.findAll();

  for (const auto& client : clients) {
    if (insura::domain::strops::contains(client.getFirstName(), search_term) ||
        insura::domain::strops::contains(client.getLastName(), search_term)) {
      found.push_back(client);
    }
  }

  return found;
}

}  // namespace insura::service
