#pragma once
#include "../domain/client.hpp"
#include "../domain/i_client_repository.hpp"

namespace insura::data {

class CSVClientRepository : public domain::IClientRepository {
 public:
  explicit CSVClientRepository(const std::string& filepath);
  void load();
  void insertClient(domain::Client client) override;
  void removeClient(const std::string& uuid) override;
  void updateClient(domain::Client updated) override;
  std::optional<domain::Client> findByUuid(
      const std::string& uuid) const override;
  std::optional<domain::Client> findByEmail(
      const std::string& email) const override;
  const std::vector<domain::Client>& findAll() const override;

 private:
  void save() const override;
  std::string serialize(const domain::Client& c) const;
  domain::Client deserialize(const std::string& line) const;
  std::vector<domain::Client> clients_;
  std::string filepath_;
  bool dirty_ = false;
  ;
};
}  // namespace insura::data
