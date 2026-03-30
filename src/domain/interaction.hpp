#pragma once
#include <string>
/*
 * Design considerations — Interaction hierarchy
 *
 * Interaction is an abstract base class with a pure virtual display()
 * method. This prevents direct instantiation — only Appointment and
 * Contract can be created. The service layer is responsible for
 * deciding which concrete type to construct based on user input.
 *
 * Virtual destructor is required on any base class. Without it,
 * deleting a derived object through a base pointer calls only the
 * base destructor, causing undefined behavior and potential leaks.
 *
 * display() is declared const and pure virtual. const because
 * rendering never modifies the object. Pure virtual because every
 * concrete subtype displays differently — Appointment shows time
 * and outcome, Contract shows value and status. Derived classes
 * use override to let the compiler verify the signature matches.
 *
 * Getters for primitive types (double, enum) return by value — no
 * reference needed since copying a primitive costs nothing and a
 * const reference on a primitive communicates nothing meaningful.
 * Getters for strings return const& to avoid copies while
 * preventing the caller from modifying internal state.
 *
 * Contract::getSignedDate() is named explicitly to distinguish it
 * from Interaction::getDate() which returns the interaction date.
 * Both exist and mean different things — naming makes that clear.
 *
 * uuid_ and created_at_ are generated at construction and never
 * modified — no setters for these fields.
 *
 * client_uuid_ is the foreign reference back to the owning Client.
 * Interaction never knows the client email — the service resolves
 * email to uuid before constructing any Interaction subtype.
 */

namespace insura::domain {

class Interaction {
 public:
  enum class InteractionType {
    APPOINTMENT,
    CONTRACT,
  };

  virtual void display() const = 0;
  Interaction(std::string client_uuid, InteractionType type, std::string date);
  virtual ~Interaction() = default;

  const std::string& getUuid() const;
  const std::string& getClientUuid() const;
  InteractionType getType() const;
  const std::string& getDate() const;
  const std::string& getNotes() const;

 private:
  std::string uuid_;
  std::string client_uuid_;
  InteractionType interaction_type_;
  std::string date_;  // TODO: evaluate format
  std::string notes_;
  std::string created_at_;  // same todo for date
  std::string updated_at_;
};

class Appointment : public Interaction {
 public:
  void display() const override;
  Appointment(std::string client_uuid, std::string date, std::string time);
  const std::string& getTime() const;
  const std::string& getReport() const;

 private:
  std::string time_;
  std::string report_;
};
class Contract : public Interaction {
 public:
  enum class ContractStatus {
    DRAFT,
    SIGNED,
    ACTIVE,
    TERMINATED,
  };

  Contract(std::string client_uuid, std::string date, double value,
           std::string product_name, std::string signed_date,
           ContractStatus status);
  void display() const override;
  double getValue() const;
  const std::string& getProductName() const;
  const std::string& getSignedDate() const;
  ContractStatus getStatus() const;

 private:
  double value_;
  std::string product_name_;
  std::string signed_date_;
  ContractStatus contract_status_;
};

}  // namespace insura::domain
