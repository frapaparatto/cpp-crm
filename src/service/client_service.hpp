#pragma once
#include "../domain/client_data.hpp"
#include "../domain/i_client_repository.hpp"

namespace insura::service {

class ClientService {
 public:
  ClientService(domain::IClientRepository& repo) : repo_(repo) {}
  void addClient(const domain::ClientData& client_data);

  void deleteClient(const std::string& uuid);
  void editClient(const std::string& uuid,
                  const domain::ClientData& new_client_data);
  std::vector<domain::Client> searchClients(const std::string& search_term);

 private:
  bool isEmailUnique(const std::string& email);
  domain::IClientRepository& repo_;
};

}  // namespace insura::service
