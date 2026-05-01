#include "application.hpp"

#include <iostream>

#include "client_controller.hpp"
#include "menu.hpp"

namespace insura::cli {

Application::Application(service::ClientService& client_service,
                         domain::IClientRepository& repo) {
  controllers_["clients"] =
      std::make_unique<ClientController>(client_service, repo);
}

void Application::cmdConfig() { std::cout << "Feature not implemented yet!"; }

void Application::cmdClear() { std::cout << "\033[2J\033[H"; }

void Application::cmdSave() {
  for (auto& [name, controller] : controllers_) {
    controller->save();
  }
  std::cout << "  Data saved correctly.\n";
}

void Application::cmdExit() {
  std::cout << "Closing session.\n";
  running_ = false;
}

bool Application::handleAppCmds(const std::string& cmd) {
  if (cmd == "save") {
    cmdSave();
    return true;
  } else if (cmd == "exit") {
    cmdExit();
    return true;
  } else if (cmd == "clear") {
    cmdClear();
    return true;
  }
  return false;
}

void Application::run() {
  running_ = true;
  active_controller_ = nullptr;

  while (running_) {
    std::string ctrl;
    std::string option;
    Menu::displayInitMenu();
    std::cout << "> ";
    std::getline(std::cin, ctrl);

    /* handleAppCmds lets the user run app-level commands
     * (save, exit, clear)
     * from any menu, avoiding duplication and keeping those responsibilities
     * out of the controllers.*/
    if (handleAppCmds(ctrl)) {
      continue;
    }

    auto it = controllers_.find(ctrl);
    if (it != controllers_.end()) {
      active_controller_ = it->second.get();

      while (running_) {
        Menu::displayEntityMenu();
        std::cout << "> ";
        std::getline(std::cin, option);

        if (option == "back") break;
        if (!handleAppCmds(option)) {
          active_controller_->execute(option);
        }
      }

    } else {
      std::cout << "  Unknown command. Type a command from the menu above.\n";
    }
  }
}

}  // namespace insura::cli
