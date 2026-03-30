#pragma once
#include <memory>
#include <string>
#include <vector>

#include "interaction.hpp"

/*
 * IInteractionRepository — abstract interface for interaction data access.
 *
 * Interaction is an abstract base class — it cannot be instantiated or
 * copied. This affects the interface design in two ways:
 *
 * insertInteraction and updateInteraction accept
 * const std::unique_ptr<Interaction>& instead of const Interaction&.
 * Passing by reference would work for reading but the repository needs
 * to store the full derived object (Contract or Appointment). Storing
 * requires a pointer — a unique_ptr preserves the concrete type on the
 * heap without slicing or copying the abstract base.
 *
 * All find and filter methods return
 * std::vector<std::unique_ptr<Interaction>> for the same reason.
 * std::vector<Interaction> is forbidden because Interaction is abstract
 * and cannot be placed in a vector by value.
 *
 * filterByType and filterByDate return an empty vector when no results
 * match — std::optional is not used because an empty result is not a
 * failure, it is a valid answer.
 *
 * findByClientUuid returns all interactions for a given client since
 * a client can have multiple interactions.
 *
 * Virtual destructor is required — without it, deleting a derived
 * repository through this interface pointer causes undefined behavior.
 */

namespace insura::domain {

class IInteractionRepository {
 public:
  virtual void insertInteraction(
      const std::unique_ptr<Interaction>& interaction) = 0;
  virtual void removeInteraction(const std::string& uuid) = 0;
  virtual void updateInteraction(
      const std::string& uuid,
      const std::unique_ptr<Interaction>& updated) = 0;

  virtual std::vector<std::unique_ptr<Interaction>> filterByType(
      Interaction::InteractionType type) = 0;
  virtual std::vector<std::unique_ptr<Interaction>> filterByDate(
      const std::string& start_date, const std::string& end_date) = 0;
  virtual std::vector<std::unique_ptr<Interaction>> findByClientUuid(
      const std::string& client_uuid) = 0;
  virtual std::vector<std::unique_ptr<Interaction>> findAll() = 0;

  virtual ~IInteractionRepository() = default;
};

}  // namespace insura::domain
