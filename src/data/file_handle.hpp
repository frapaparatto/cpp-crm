#include <string>
#include <fstream>

namespace insura::data {
class FileHandler {
 public:
  FileHandler(std::string filepath, std::ios::openmode mode);
  ~FileHandler();

 private:
  std::fstream file_;
};

}  // namespace insura::data
