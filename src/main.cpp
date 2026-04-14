#include <exception>
#include <iostream>
#include <memory>

#include "application.hpp"
#include "client_service.hpp"
#include "csv_client_repository.hpp"

// TODO: address autocomplete via geocoding API (Google Places or Nominatim) -
// implement after TUI search is complete
// TODO: at the end of the project, adjust all docs journal notes and remove the
// - that AI added in each note
// TODO: add in the readme the section that explains how I used AI

/* Default filepath used when the user starts a fresh session without specifying
 * one. Hardcoded for now.
 * TODO: replace with a value read from a config file (e.g. insura.conf or
 * ~/.insura/config). The config should store at minimum: default_filepath and
 * optionally last_opened_filepath. main.cpp reads it here and passes the
 * resolved path down — the repo stays unaware of any config. */
static constexpr const char* kDefaultFilepath = "insura_data.csv";

int main() {
  int option;
  std::cout << "Options\n";
  std::cout << "1. New\n";
  std::cout << "2. Load\n";
  std::cin >> option;

  std::unique_ptr<insura::data::CSVClientRepository> repo;

  if (option == 1) {
    std::string filepath;
    std::cout << "Enter filepath for new CRM: ";
    std::cin >> filepath;

    repo = std::make_unique<insura::data::CSVClientRepository>(filepath);

  } else if (option == 2) {
    while (true) {
      std::string filepath;
      std::cout << "Enter filepath to load: ";
      std::cin >> filepath;

      repo = std::make_unique<insura::data::CSVClientRepository>(filepath);

      try {
        repo->load();
        break;

      } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cout << "1. Try again\n";
        std::cout << "2. Start empty\n";

        int retry;
        std::cin >> retry;
        if (retry == 1) {
          continue;
        } else if (retry == 2) {
          repo = std::make_unique<insura::data::CSVClientRepository>(
              kDefaultFilepath);
          break;
        } else {
          std::cerr << "Invalid option\n";
          continue;
        }
      }
    }
  } else {
    std::cerr << "Invalid option\n";
    return 1;
  }

  insura::service::ClientService client_service(*repo);
  insura::cli::Application app(client_service, *repo);

  app.run();

  return 0;
}
