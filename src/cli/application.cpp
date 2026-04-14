#include "application.hpp"

#include <iostream>
#include <regex>

#include "client_service.hpp"
#include "client_view.hpp"
#include "menu.hpp"

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

bool isValidEmail(const std::string& email) {
  const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
  return std::regex_match(email, pattern);
}

bool isDigitsOnly(const std::string& str) {
  return !str.empty() &&
         std::find_if(str.begin(), str.end(), [](unsigned char c) {
           return !std::isdigit(c);
         }) == str.end();
}

bool isValidPhone(const std::string& phone) {
  /* TODO: standardise phone display: add country-code prefix (e.g. +39)
   * and decide whether to keep phone as std::string or introduce a dedicated
   * type/format. Handle at end-of-project cleanup. */
  return isDigitsOnly(phone);
}

std::string promptRequired(const std::string& label) {
  std::string value;
  do {
    std::cout << label;
    std::getline(std::cin, value);
    if (value.empty()) std::cout << "  This field is required.\n";
  } while (value.empty());
  return value;
}

std::optional<std::string> promptOptional(const std::string& label) {
  std::string value;
  std::cout << label;
  std::getline(std::cin, value);
  if (value.empty()) return std::nullopt;
  return value;
}

}  // namespace

namespace insura::cli {

Application::Application(service::ClientService& client_service,
                         domain::IClientRepository& repo)
    : client_service_(client_service), repo_(repo) {
  initCommands();
}

void Application::initCommands() {
  commands_["add"] = [this]() { cmdAdd(); };
  commands_["list"] = [this]() { cmdList(); };
  commands_["search"] = [this]() { cmdSearch(); };
  commands_["edit"] = [this]() { cmdEdit(); };
  commands_["delete"] = [this]() { cmdDelete(); };
  commands_["save"] = [this]() { cmdSave(); };
  commands_["exit"] = [this]() { cmdExit(); };
}

void Application::cmdAdd() {
  domain::ClientData data;

  /* TODO: this could be better handled using iomanip */
  std::cout << "--- Add New Client ---\n";

  /* Required fields */
  data.first_name = promptRequired("First name: ");
  data.last_name = promptRequired("Last name: ");

  while (true) {
    std::cout << "Email: ";
    std::string email;
    std::getline(std::cin, email);
    if (email.empty()) {
      std::cout << "  This field is required.\n";
      continue;
    }
    if (!isValidEmail(email)) {
      std::cout << "  Invalid email format.\n";
      continue;
    }
    data.email = std::move(email);
    break;
  }

  /* Optional fields */
  while (true) {
    auto phone = promptOptional("Phone (optional, digits only): ");
    if (!phone.has_value()) break;
    if (!isValidPhone(*phone)) {
      std::cout << "  Phone must contain digits only.\n";
      continue;
    }
    data.phone = std::move(phone);
    break;
  }

  data.job_title = promptOptional("Job title (optional): ");
  data.company = promptOptional("Company (optional): ");
  data.address = promptOptional("Address (optional): ");
  data.city = promptOptional("City (optional): ");

  while (true) {
    auto postal_code = promptOptional("Postal code (optional, digits only): ");
    if (!postal_code.has_value()) break;
    if (!isDigitsOnly(*postal_code)) {
      std::cout << "  Postal code must contain digits only.\n";
      continue;
    }
    data.postal_code = std::move(postal_code);
    break;
  }

  data.notes = promptOptional("Notes (optional): ");

  try {
    client_service_.addClient(data);
    std::cout << "Client added successfully.\n";
  } catch (const std::invalid_argument& e) {
    std::cout << "  Error: " << e.what() << "\n";
  }
}

insura::domain::Client Application::selectClient(
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
    if (!isDigitsOnly(input)) {
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

std::optional<domain::Client> Application::resolveClient() {
  std::string term;
  do {
    std::cout << "Search: ";
    std::getline(std::cin, term);

    /* TODO: I should decide if when the search term is leaved empty
     * I have to ask the user if he wants to exit or re-prompting.
     * I don't know if it is necessary since if the user select the
     * option, probably he wants to search something but maybe
     * it select the wrong option for a typing error */
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

void Application::cmdSearch() {
  auto client = resolveClient();
  if (client) ClientView::displayOne(*client);
}

domain::ClientData Application::promptEditData(const domain::Client& current) {
  domain::ClientData updated_data;

  /* Required fields — shown with current value; leave blank to keep */
  std::optional<std::string> first =
      promptOptional("First name [" + current.getFirstName() + "]: ");
  if (first) updated_data.first_name = std::move(*first);

  std::optional<std::string> last =
      promptOptional("Last name [" + current.getLastName() + "]: ");
  if (last) updated_data.last_name = std::move(*last);

  while (true) {
    std::optional<std::string> email =
        promptOptional("Email [" + current.getEmail() + "]: ");
    if (!email)
      break;
    else if (!isValidEmail(*email)) {
      std::cout << "  Invalid email format.\n";
      continue;
    } else {
      updated_data.email = std::move(*email);
      break;
    }
  }

  while (true) {
    std::string prompt = current.getPhone()
                             ? "Phone [" + current.getPhone().value() + "]: "
                             : "Phone (optional): ";

    auto phone = promptOptional(prompt);
    if (!phone.has_value()) break;

    if (!isValidPhone(*phone)) {
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
    updated_data.job_title = promptOptional(prompt);
  }

  {
    std::string prompt =
        current.getCompany()
            ? "Company [" + current.getCompany().value() + "]: "
            : "Company (optional): ";
    updated_data.company = promptOptional(prompt);
  }

  {
    std::string prompt =
        current.getAddress()
            ? "Address [" + current.getAddress().value() + "]: "
            : "Address (optional): ";
    updated_data.address = promptOptional(prompt);
  }

  {
    std::string prompt = current.getCity()
                             ? "City [" + current.getCity().value() + "]: "
                             : "City (optional): ";
    updated_data.city = promptOptional(prompt);
  }

  while (true) {
    std::string prompt =
        current.getPostalCode()
            ? "Postal code [" + current.getPostalCode().value() + "]: "
            : "Postal code (optional, digits only): ";
    auto postal_code = promptOptional(prompt);
    if (!postal_code.has_value()) break;
    if (!isDigitsOnly(*postal_code)) {
      std::cout << "  Postal code must contain digits only.\n";
      continue;
    }
    updated_data.postal_code = std::move(postal_code);
    break;
  }

  {
    std::string prompt = current.getNotes()
                             ? "Notes [" + current.getNotes().value() + "]: "
                             : "Notes (optional): ";
    updated_data.notes = promptOptional(prompt);
  }

  return updated_data;
}

void Application::cmdList() { ClientView::displayAll(repo_.findAll()); }

void Application::cmdDelete() {
  auto client = resolveClient();
  if (!client) return;

  try {
    client_service_.deleteClient(client->getUuid());
  } catch (const std::invalid_argument& e) {
    std::cout << " Error: " << e.what() << "\n";
  }
}

void Application::cmdEdit() {
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

void Application::cmdSave() { repo_.save(); }

void Application::cmdExit() {
  std::cout << "Closing session.\n";
  running_ = false;
}

void Application::run() {
  running_ = true;
  Menu::display();

  while (running_) {
    std::string option;
    std::cout << "> ";
    std::getline(std::cin, option);

    auto it = commands_.find(option);
    if (it != commands_.end()) {
      it->second();
    } else {
      std::cout << "  Unknown command. Type a command from the menu above.\n";
    }
  }
}

}  // namespace insura::cli
