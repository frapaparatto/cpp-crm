#pragma once
#include <functional>
#include <unordered_map>

#include "../domain/i_client_repository.hpp"
#include "../service/client_service.hpp"

namespace insura::cli {

class Application {
 public:
  Application(service::ClientService& client_service,
              domain::IClientRepository& repo);
  void run();

 private:
  bool running_ = false;
  service::ClientService& client_service_;
  domain::IClientRepository& repo_;
  std::unordered_map<std::string, std::function<void()>> commands_;

  /* Helpers */
  void initCommands();
  domain::Client selectClient(const std::vector<domain::Client>& clients);
  std::optional<domain::Client> resolveClient();
  domain::ClientData promptEditData(const domain::Client& current);

  /* Commands */
  void cmdAdd();
  void cmdList();
  void cmdSearch();
  void cmdEdit();
  void cmdDelete();
  void cmdSave();
  void cmdExit();
};

}  // namespace insura::cli
