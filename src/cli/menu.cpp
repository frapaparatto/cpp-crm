#include "menu.hpp"

#include <iomanip>
#include <iostream>
#include <string>

namespace insura::cli {

namespace {
constexpr int kCmdWidth = 10;
const std::string kSeparator(kCmdWidth + 30, '-');
}  // namespace

void Menu::display() {
  std::cout << "\nInsuraPro CRM\n"
            << kSeparator << '\n'
            << std::left << std::setw(kCmdWidth) << "add"    << "— add a new client\n"
            << std::setw(kCmdWidth) << "search" << "— search clients\n"
            << std::setw(kCmdWidth) << "list"   << "— list all clients\n"
            << std::setw(kCmdWidth) << "view"   << "— view a client's full record\n"
            << std::setw(kCmdWidth) << "edit"   << "— edit a client\n"
            << std::setw(kCmdWidth) << "delete" << "— delete a client\n"
            << std::setw(kCmdWidth) << "save"   << "— save data to CSV\n"
            << std::setw(kCmdWidth) << "config" << "— view or edit application configuration\n"
            << std::setw(kCmdWidth) << "clear"  << "— clear the terminal screen\n"
            << std::setw(kCmdWidth) << "exit"   << "— exit\n"
            << kSeparator << '\n';
}

}  // namespace insura::cli
