#pragma once
#include <string_view>

namespace insura::cli {

class Menu {
 public:
  static void displayInitMenu();
  static void displayEntityMenu(std::string_view entity);
};

}  // namespace insura::cli
