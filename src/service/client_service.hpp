#pragma once
#include "../domain/client_data.hpp"
#include "../domain/i_client_repository.hpp"

namespace insura::service {

class ClientService {
 public:
  explicit ClientService(domain::IClientRepository& repo) : repo_(repo) {}
  void addClient(const domain::ClientData& client_data);

  void deleteClient(std::string_view uuid);
  void editClient(std::string_view uuid,
                  const domain::ClientData& new_client_data);
  std::vector<domain::Client> searchClients(
std::string_view search_term) const;

 private:
  bool isEmailUnique(std::string_view email) const;
  domain::IClientRepository& repo_;
};

}  // namespace insura::service
