#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "i_entity_controller.hpp"
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
  std::unordered_map<std::string, std::unique_ptr<IEntityController>> controllers_;
  IEntityController* active_controller_ = nullptr;
  bool handleAppCmds(const std::string& option);

  /* Application-level commands */
  void cmdSave();
  void cmdExit();
  void cmdClear();
  void cmdConfig();
};

}  // namespace insura::cli
