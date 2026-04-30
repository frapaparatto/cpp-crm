#include "menu.hpp"

#include <iomanip>
#include <iostream>
#include <string>

namespace insura::cli {

namespace {
constexpr int kCmdWidth = 16;
const std::string kSeparator(kCmdWidth + 30, '-');
}  // namespace

void Menu::displayInitMenu() {
  std::cout << "\nInsuraPro CRM\n"
            << kSeparator << '\n'
            << std::left << std::setw(kCmdWidth) << "clients"
            << "manage clients\n"
            << std::setw(kCmdWidth) << "policies"
            << "manage policies (coming soon)\n"
            << std::setw(kCmdWidth) << "interactions"
            << "manage interactions (coming soon)\n"
            << std::setw(kCmdWidth) << "save" << "save data to CSV\n"
            << std::setw(kCmdWidth) << "exit" << "exit\n"
            << kSeparator << '\n';
}

void Menu::displayEntityMenu() {
  /*
   * Kept generic intentionally: the user selects the menu
   * so they already know which data they're working on.
   * Not optimal: a contextual header like "Clients Menu"
   * would improve clarity. (Will handle it later)
   */
  std::cout << "\nInsuraPro CRM\n"
            << kSeparator << '\n'
            << std::left << std::setw(kCmdWidth) << "add"
            << "add a new record\n"
            << std::setw(kCmdWidth) << "search" << "search records\n"
            << std::setw(kCmdWidth) << "list" << "list all records\n"
            << std::setw(kCmdWidth) << "view"
            << "view a record's full details\n"
            << std::setw(kCmdWidth) << "edit" << "edit a record\n"
            << std::setw(kCmdWidth) << "delete" << "delete a record\n"
            << std::setw(kCmdWidth) << "back" << "back to main menu\n"
            << std::setw(kCmdWidth) << "save" << "save data to CSV\n"
            << std::setw(kCmdWidth) << "exit" << "exit\n"
            << kSeparator << '\n';
}

}  // namespace insura::cli
