#include "client_controller.hpp"

#include <iostream>

#include "../domain/client_status.hpp"
#include "../domain/strops.hpp"
#include "./client_view.hpp"
#include "utils.hpp"

/*
 * Note before starting: I won't care about the menu and options
 * I will improve them using AI or understanding how to
 * write better descriptions but for now I want to be fast and
 * focus on the main work.
 */

namespace {

/* CLI-layer format validators: kept here for immediate re-prompt UX.
 * Domain-layer checks in client.cpp remain the authoritative last line of
 * defence and must not be removed. */

std::string promptRequired(std::string_view prompt) {
  std::string value;
  do {
    std::cout << prompt;
    std::getline(std::cin, value);
    value = insura::domain::strops::trim(value);
    if (value.empty()) std::cout << "  This field is required.\n";
  } while (value.empty());
  return value;
}

std::optional<std::string> promptOptional(std::string_view prompt) {
  std::string value;
  std::cout << prompt;
  std::getline(std::cin, value);
  value = insura::domain::strops::trim(value);
  if (value.empty()) return std::nullopt;
  return value;
}

/* ordered to match enum declaration; index i corresponds to menu choice i+1 */
constexpr insura::domain::Client::ClientStatus kStatusOptions[] = {
    insura::domain::Client::ClientStatus::NEW,
    insura::domain::Client::ClientStatus::CONTACTED,
    insura::domain::Client::ClientStatus::IN_PROGRESS,
    insura::domain::Client::ClientStatus::OPEN_DEAL,
    insura::domain::Client::ClientStatus::ATTEMPTED_TO_CONTACT,
    insura::domain::Client::ClientStatus::CLOSED_WON,
    insura::domain::Client::ClientStatus::CLOSED_LOST,
};
constexpr int kStatusCount = std::size(kStatusOptions);

std::optional<insura::domain::Client::ClientStatus> promptStatus() {
  for (int i = 0; i < kStatusCount; ++i) {
    std::cout << "  " << (i + 1) << ". "
              << insura::domain::statusToString(kStatusOptions[i]);
    if (kStatusOptions[i] == insura::domain::Client::ClientStatus::NEW)
      std::cout << " (default)";
    std::cout << "\n";
  }
  while (true) {
    std::cout << "Select status (1-7, Press Enter for default): ";
    std::string input;
    std::getline(std::cin, input);
    input = insura::domain::strops::trim(input);
    if (input.empty()) return std::nullopt;
    if (input.size() == 1 && input[0] >= '1' && input[0] <= '7') {
      return kStatusOptions[static_cast<std::size_t>(input[0] - '1')];
    }
    std::cout << "  Please enter a number between 1 and 7.\n";
  }
}

}  // namespace

namespace insura::cli {

ClientController::ClientController(service::ClientService& client_service,
                                   domain::IClientRepository& repo)
    : client_service_(client_service), repo_(repo) {
  commands_["add"] = [this]() { cmdAdd(); };
  commands_["search"] = [this]() { cmdSearch(); };
  commands_["list"] = [this]() { cmdList(); };
  commands_["view"] = [this]() { cmdView(); };
  commands_["edit"] = [this]() { cmdEdit(); };
  commands_["delete"] = [this]() { cmdDelete(); };
}

void ClientController::save() { repo_.save(); }

void ClientController::execute(const std::string& cmd) {
  auto it = commands_.find(cmd);

  if (it != commands_.end())
    it->second();
  else
    std::cout << "\nUnknown commands\n";
}

void ClientController::cmdView() {
  std::cout << "Feature not implemented yet!";
}

void ClientController::cmdAdd() {
  domain::ClientData data;

  /* Required fields */
  std::string first = promptRequired("First name: ");
  data.first_name = insura::domain::strops::capitalize(first);
  std::string last = promptRequired("Last name: ");
  data.last_name = insura::domain::strops::capitalize(last);

  while (true) {
    std::cout << "Email: ";
    std::string email;
    std::getline(std::cin, email);
    email = insura::domain::strops::trim(email);
    if (email.empty()) {
      std::cout << "  This field is required.\n";
      continue;
    }
    if (!insura::utils::isValidEmail(email)) {
      std::cout << "  Invalid email format.\n";
      continue;
    }
    data.email = insura::domain::strops::lower(email);
    break;
  }

  /* Optional fields */
  while (true) {
    auto phone = promptOptional("Phone (optional, digits only): ");
    if (!phone.has_value()) break;
    if (!insura::utils::isValidPhone(*phone)) {
      std::cout << "  Phone must contain digits only.\n";
      continue;
    }
    data.phone = std::move(phone);
    break;
  }

  std::optional<std::string> job = promptOptional("Job title (optional): ");

  data.job_title = job ? insura::domain::strops::capitalize(*job) : job;

  std::optional<std::string> company = promptOptional("Company (optional): ");
  data.company =
      company ? insura::domain::strops::capitalize(*company) : company;

  auto address = promptOptional("Address (optional): ");
  data.address =
      address ? insura::domain::strops::capitalize(*address) : address;
  std::optional<std::string> city = promptOptional("City (optional): ");
  data.city = city ? insura::domain::strops::capitalize(*city) : city;

  while (true) {
    auto postal_code = promptOptional("Postal code (optional, digits only): ");
    if (!postal_code.has_value()) break;
    if (!insura::utils::isDigitsOnly(*postal_code)) {
      std::cout << "  Postal code must contain digits only.\n";
      continue;
    }
    data.postal_code = std::move(postal_code);
    break;
  }

  std::cout << "Status:\n";
  data.lead_status = promptStatus();

  auto notes = promptOptional("Notes (optional): ");
  data.notes = notes ? insura::domain::strops::capitalize(*notes) : notes;

  try {
    client_service_.addClient(data);
    std::cout << "Client added successfully.\n";
  } catch (const std::invalid_argument& e) {
    std::cout << "  Error: " << e.what() << "\n";
  }
}

insura::domain::Client ClientController::selectClient(
    const std::vector<insura::domain::Client>& clients) {
  for (std::size_t i = 0; i < clients.size(); ++i) {
    std::cout << "  [" << (i + 1) << "] " << clients[i].getFirstName() << " "
              << clients[i].getLastName() << " — " << clients[i].getEmail()
              << '\n';
  }

  while (true) {
    std::cout << "Select a client (1-" << clients.size() << "): ";
    std::string input;
    std::getline(std::cin, input);
    if (!insura::utils::isDigitsOnly(input)) {
      std::cout << "  Please enter a valid number.\n";
      continue;
    }
    int index = std::stoi(input);
    if (index < 1 || index > static_cast<int>(clients.size())) {
      std::cout << "  Number out of range.\n";
      continue;
    }
    return clients[static_cast<std::size_t>(index - 1)];
  }
}

std::optional<domain::Client> ClientController::resolveClient() {
  std::string term;
  do {
    std::cout << "Search: ";
    std::getline(std::cin, term);

    /* TODO: I should decide if when the search term is leaved empty
     * I have to ask the user if he wants to exit or re-prompting.
     * I don't know if it is necessary since if the user select the
     * option, probably he wants to search something but maybe
     * it select the wrong option for a typing error */
    term = insura::domain::strops::trim(term);
    if (term.empty()) continue;

    std::vector<domain::Client> found = client_service_.searchClients(term);

    if (found.empty()) {
      std::cout << "No Contact Found\n";

    } else if (found.size() == 1) {
      return found.at(0);

    } else {
      domain::Client selected = selectClient(found);
      return selected;
    }

  } while (term.empty());

  return std::nullopt;
}

void ClientController::cmdSearch() {
  auto client = resolveClient();
  if (client) ClientView::displayOne(*client);
}

/* This function both prompts the user for updated field values and populates
 * the ClientData struct — intentionally kept together, same pattern as
 * cmdAdd. If a second non-CLI caller appears, the two responsibilities can
 * be separated at that point. */
domain::ClientData ClientController::promptEditData(
    const domain::Client& current) {
  domain::ClientData updated_data;

  {
    /* Required fields — shown with current value; leave blank to keep */
    std::optional<std::string> first =
        promptOptional("First name [" + current.getFirstName() + "]: ");
    if (first)
      updated_data.first_name = insura::domain::strops::capitalize(*first);
  }

  {
    std::optional<std::string> last =
        promptOptional("Last name [" + current.getLastName() + "]: ");
    if (last)
      updated_data.last_name = insura::domain::strops::capitalize(*last);
  }

  while (true) {
    std::optional<std::string> email =
        promptOptional("Email [" + current.getEmail() + "]: ");
    if (!email)
      break;
    else if (!insura::utils::isValidEmail(*email)) {
      std::cout << "  Invalid email format.\n";
      continue;
    } else {
      updated_data.email = insura::domain::strops::lower(*email);
      break;
    }
  }

  while (true) {
    std::string prompt = current.getPhone()
                             ? "Phone [" + current.getPhone().value() + "]: "
                             : "Phone (optional): ";

    auto phone = promptOptional(prompt);
    if (!phone.has_value()) break;

    if (!insura::utils::isValidPhone(*phone)) {
      std::cout << "  Phone must contain digits only.\n";
      continue;
    }
    updated_data.phone = std::move(phone);
    break;
  }

  {
    std::string prompt =
        current.getJobTitle()
            ? "Job title [" + current.getJobTitle().value() + "]: "
            : "Job title (optional): ";
    auto job_title = promptOptional(prompt);
    updated_data.job_title =
        job_title ? insura::domain::strops::capitalize(*job_title) : job_title;
  }

  {
    std::string prompt =
        current.getCompany()
            ? "Company [" + current.getCompany().value() + "]: "
            : "Company (optional): ";
    auto company = promptOptional(prompt);
    updated_data.company =
        company ? insura::domain::strops::capitalize(*company) : company;
  }

  {
    std::string prompt =
        current.getAddress()
            ? "Address [" + current.getAddress().value() + "]: "
            : "Address (optional): ";
    auto address = promptOptional(prompt);
    updated_data.address =
        address ? insura::domain::strops::capitalize(*address) : address;
  }

  {
    std::string prompt = current.getCity()
                             ? "City [" + current.getCity().value() + "]: "
                             : "City (optional): ";
    auto city = promptOptional(prompt);
    updated_data.city = city ? insura::domain::strops::capitalize(*city) : city;
  }

  while (true) {
    std::string prompt =
        current.getPostalCode()
            ? "Postal code [" + current.getPostalCode().value() + "]: "
            : "Postal code (optional, digits only): ";
    auto postal_code = promptOptional(prompt);
    if (!postal_code.has_value()) break;
    if (!insura::utils::isDigitsOnly(*postal_code)) {
      std::cout << "  Postal code must contain digits only.\n";
      continue;
    }
    updated_data.postal_code = std::move(postal_code);
    break;
  }

  {
    std::cout << "Status ["
              << insura::domain::statusToString(current.getStatus())
              << "] (1-7 to change, Enter to keep):\n";
    for (int i = 0; i < kStatusCount; ++i) {
      std::cout << "  " << (i + 1) << ". "
                << insura::domain::statusToString(kStatusOptions[i]) << "\n";
    }
    while (true) {
      std::cout << "> ";
      std::string input;
      std::getline(std::cin, input);
      input = insura::domain::strops::trim(input);
      if (input.empty()) break;
      if (input.size() == 1 && input[0] >= '1' && input[0] <= '7') {
        updated_data.lead_status =
            kStatusOptions[static_cast<std::size_t>(input[0] - '1')];
        break;
      }
      std::cout << "  Please enter a number between 1 and 7, or press Enter "
                   "to keep.\n";
    }
  }

  {
    std::string prompt = current.getNotes()
                             ? "Notes [" + current.getNotes().value() + "]: "
                             : "Notes (optional): ";
    auto notes = promptOptional(prompt);
    updated_data.notes =
        notes ? insura::domain::strops::capitalize(*notes) : notes;
  }

  return updated_data;
}

void ClientController::cmdList() { ClientView::displayAll(repo_.findAll()); }

void ClientController::cmdDelete() {
  auto client = resolveClient();
  if (!client) return;

  ClientView::displayOne(*client);
  std::string choice;
  std::cout << "\nAre you sure? (Y/n): ";
  std::getline(std::cin, choice);

  if (choice == "n") return;

  try {
    client_service_.deleteClient(client->getUuid());
    std::cout << "Client eliminated successfully.\n";
  } catch (const std::invalid_argument& e) {
    std::cout << " Error: " << e.what() << "\n";
  }
}

void ClientController::cmdEdit() {
  auto client = resolveClient();
  if (!client) return;

  domain::ClientData updated = promptEditData(*client);
  try {
    client_service_.editClient(client->getUuid(), updated);
    std::cout << "Client updated successfully.\n";
  } catch (const std::invalid_argument& e) {
    std::cout << "  Error: " << e.what() << "\n";
  }
}

}  // namespace insura::cli
