# InsuraPro CRM — File Inventory

## domain — insura::domain

| File | Responsibility |
|---|---|
| `client.hpp` | Declares the Client class with private fields, a constructor that enforces field-level invariants, and public accessors |
| `client.cpp` | Implements Client constructor validation and any non-trivial accessor logic |
| `policy.hpp` | Declares the Policy class with private fields, constructor, and public accessors — holds client_uuid as owner reference |
| `policy.cpp` | Implements Policy constructor and accessor logic |
| `interaction.hpp` | Declares the abstract Interaction base class and the concrete Appointment and Contract derived classes with their own fields and virtual display() |
| `interaction.cpp` | Implements Interaction, Appointment, and Contract constructors and any shared logic |
| `i_client_repository.hpp` | Declares the IClientRepository abstract interface — pure virtual methods for insert, remove, update, findByUuid, findByEmail, findAll |
| `i_policy_repository.hpp` | Declares the IPolicyRepository abstract interface — pure virtual methods for policy CRUD plus findByClientUuid |
| `i_interaction_repository.hpp` | Declares the IInteractionRepository abstract interface — pure virtual methods for interaction CRUD plus findByClientUuid |

## data — insura::data

| File | Responsibility |
|---|---|
| `csv_client_repository.hpp` | Declares CsvClientRepository as a concrete implementation of IClientRepository backed by CSV storage |
| `csv_client_repository.cpp` | Implements all IClientRepository methods — reads and writes client data to CSV using the RAII file handle |
| `csv_policy_repository.hpp` | Declares CsvPolicyRepository as a concrete implementation of IPolicyRepository |
| `csv_policy_repository.cpp` | Implements all IPolicyRepository methods against CSV storage |
| `csv_interaction_repository.hpp` | Declares CsvInteractionRepository as a concrete implementation of IInteractionRepository |
| `csv_interaction_repository.cpp` | Implements all IInteractionRepository methods against CSV storage, handling both Appointment and Contract serialization |
| `file_handle.hpp` | Declares the RaiiFileHandle class — opens a file in the constructor, closes it in the destructor, no manual resource management needed by callers |
| `file_handle.cpp` | Implements RaiiFileHandle constructor, destructor, and any file operation helpers |

## service — insura::service

| File | Responsibility |
|---|---|
| `client_service.hpp` | Declares ClientService — the orchestrator for all client operations, receives IClientRepository by reference via constructor injection |
| `client_service.cpp` | Implements all client business logic: email uniqueness enforcement, cascade delete of policies and interactions, search, CRUD |
| `policy_service.hpp` | Declares PolicyService — orchestrates policy operations, receives IPolicyRepository by reference |
| `policy_service.cpp` | Implements policy business logic: add, edit, delete, list by client |
| `interaction_service.hpp` | Declares InteractionService — orchestrates interaction operations, receives IInteractionRepository by reference |
| `interaction_service.cpp` | Implements interaction business logic: add typed interactions, filter by date and type, list by client |
| `auto_save_service.hpp` | Declares AutoSaveService — manages the background thread, mutex, and condition variable for periodic CSV persistence |
| `auto_save_service.cpp` | Implements the background thread loop, mutex-protected serialization, and clean shutdown via condition variable |

## cli — insura::cli

| File | Responsibility |
|---|---|
| `menu.hpp` | Declares MainMenu — owns the command dispatch table that maps user input strings to handler functions |
| `menu.cpp` | Implements the dispatch table and all top-level command handlers |
| `client_view.hpp` | Declares ClientView — responsible for all client-related output: formatted table list, single client detail, search results |
| `client_view.cpp` | Implements formatted rendering of client data for the terminal |
| `policy_view.hpp` | Declares PolicyView — responsible for all policy-related output: formatted policy list and single policy detail for a given client |
| `policy_view.cpp` | Implements formatted rendering of policy data for the terminal |
| `interaction_view.hpp` | Declares InteractionView — responsible for rendering interaction lists, filtered views by type and date, and single interaction detail |
| `interaction_view.cpp` | Implements formatted rendering of interaction data including polymorphic display of Appointment vs Contract |
| `application.hpp` | Declares Application — owns the main loop, handles startup flow (new/load menu), shutdown prompt, and wires all layers together |
| `application.cpp` | Implements the main loop, startup and shutdown sequences, and layer initialization |

## root

| File | Responsibility |
|---|---|
| `main.cpp` | Composition root — instantiates all concrete repositories and services, injects dependencies, creates Application and calls run() |
