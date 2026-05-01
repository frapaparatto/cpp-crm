#pragma once
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include "../domain/client.hpp"
#include "../domain/client_data.hpp"
#include "../domain/i_client_repository.hpp"
#include "../service/client_service.hpp"
#include "i_entity_controller.hpp"

namespace insura::cli {

class ClientController : public IEntityController {
 public:
  ClientController(service::ClientService& client_service,
                   domain::IClientRepository& repo);

  void save() override;
  void cmdAdd() override;
  void cmdList() override;
  void cmdSearch() override;
  void cmdView() override;
  void cmdEdit() override;
  void cmdDelete() override;
  void execute(const std::string& cmd) override;

 private:
  service::ClientService& client_service_;
  domain::IClientRepository& repo_;
  std::unordered_map<std::string, std::function<void()>> commands_;

  domain::Client selectClient(const std::vector<domain::Client>& clients);
  std::optional<domain::Client> resolveClient();
  domain::ClientData promptEditData(const domain::Client& current);
};

}  // namespace insura::cli
