#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>

#include "application.hpp"
#include "client_service.hpp"
#include "csv_client_repository.hpp"

// TODO END OF THE PROJECT: order all notes in docs/personal becasue lots of
// them needs to be used together with daily notes e.g. all pattern could became
// feynman-like files to explain pattern better. Explaining most important
// patterns could be a good idea

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
  const auto repo =
      [&]() -> std::unique_ptr<insura::data::CSVClientRepository> {

    while (true) {
      std::cout << "Options\n";
      std::cout << "1. New\n";
      std::cout << "2. Load\n";
      std::cout << "3. Exit\n";
      std::cout << "\n> ";

      std::string input;
      std::getline(std::cin, input);

      int option;
      try {
        option = std::stoi(input);
      } catch (const std::exception&) {
        std::cerr << "Invalid input, please try again.\n";
        continue;
      }

      if (option == 1) {
        std::string filepath;
        std::cout << "Enter filepath for new CRM: ";
        std::getline(std::cin, filepath);
        return std::make_unique<insura::data::CSVClientRepository>(filepath);

      } else if (option == 2) {
        while (true) {
          std::string filepath;
          std::cout << "Enter filepath to load: ";
          std::getline(std::cin, filepath);

          auto tmp_repo =
              std::make_unique<insura::data::CSVClientRepository>(filepath);

          try {
            tmp_repo->load();
            return tmp_repo;
          } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            std::cout << "1. Try again\n";
            std::cout << "2. Start empty\n";
            std::cout << "3. Exit\n";
            std::cout << "\n> ";

            std::string retry_input;
            std::getline(std::cin, retry_input);

            int retry;
            try {
              retry = std::stoi(retry_input);
            } catch (const std::exception&) {
              std::cerr << "Invalid input, please try again.\n";
              continue;
            }

            if (retry == 1) {
              continue;
            } else if (retry == 2) {
              return std::make_unique<insura::data::CSVClientRepository>(
                  kDefaultFilepath);
            } else if (retry == 3) {
              /* std::exit skips stack unwinding — local destructors do not
               * run. Safe here because no resources need flushing at this
               * point, but revisit if cleanup logic is added later. */
              std::exit(0);
            } else {
              std::cerr << "Invalid option, please try again.\n";
              continue;
            }
          }
        }

      } else if (option == 3) {
        /* std::exit skips stack unwinding, local destructors do not run.
         * Safe here because no resources need flushing at this point, but
         * revisit if cleanup logic is added later. */
        std::exit(0);
      } else {
        std::cerr << "Invalid option, please try again.\n";
      }
    }
  }();

  insura::service::ClientService client_service(*repo);
  insura::cli::Application app(client_service, *repo);
  app.run();

  return 0;
}
