#include "file_handle.hpp"

#include <iostream>

namespace insura::data {

FileHandler::FileHandler(std::string filepath, std::ios::openmode mode) {
  std::fstream file(filepath, mode);

  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file!\n");
  }

  file_ = std::move(file);
}

std::fstream& FileHandler::getStream() { return file_; }

FileHandler::~FileHandler() { file_.close(); }

}  // namespace insura::data
