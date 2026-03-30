#pragma once
#include <string>
#include <vector>

#include "policy.hpp"

/*
 * IPolicyRepository — abstract interface for policy data access.
 *
 * Policy is a concrete class so it can be passed and returned by value
 * without the slicing or abstract type problems present in
 * IInteractionRepository.
 *
 * insertPolicy and updatePolicy accept const Policy& to avoid an
 * unnecessary copy — the repository reads the object and persists it.
 *
 * findByClientUuid returns std::vector<Policy> because a client can
 * have multiple policies. std::optional is not used — an empty vector
 * already means "no policies found for this client."
 *
 * findAll returns all policies across all clients. Used by the service
 * layer when a full dataset is needed, for example during CSV save.
 *
 * Virtual destructor is required on any abstract base class.
 */

namespace insura::domain {

class IPolicyRepository {
 public:
  virtual void insertPolicy(const Policy& policy) = 0;
  virtual void removePolicy(const std::string& uuid) = 0;
  virtual void updatePolicy(const std::string& uuid,
                            const Policy& updated) = 0;

  virtual std::vector<Policy> findByClientUuid(
      const std::string& client_uuid) = 0;
  virtual std::vector<Policy> findAll() = 0;

  virtual ~IPolicyRepository() = default;
};

}  // namespace insura::domain
