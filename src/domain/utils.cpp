#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>
#include <regex>
#include <sstream>

namespace insura::domain::utils {

/*
 * UUID v4 generator implemented from scratch using std::stringstream.
 *
 * Preferred from scratch implementation in order to learn about uuid4
 * generation and string stream.
 *
 * UUID v4 structure: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 * where x is a random hex digit, 4 is fixed (version), and y is
 * one of 8, 9, a, b (RFC 4122 variant).
 *
 * std::hex tells the stream to format integers as hexadecimal, so
 * writing the integer 10 produces "a" rather than "10".
 *
 * Static variables are initialised once on the first call and reused
 * on subsequent calls to avoid recreating the generator and
 * distributions on every invocation.
 *
 * TODO: not thread-safe due to shared static state. If UUID generation
 * is needed across multiple threads, add a mutex around this function.
 * I must revisit in Milestone 3 when the auto-save thread is introduced.
 *
 */
std::string generateUuid() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  static std::uniform_int_distribution<> dist(0, 15);
  static std::uniform_int_distribution<> dist2(8, 11);

  std::stringstream ss;
  int i;
  ss << std::hex;

  for (i = 0; i < 8; i++) ss << dist(mt);
  ss << "-";
  for (i = 0; i < 4; i++) ss << dist(mt);
  ss << "-4";
  for (i = 0; i < 3; i++) ss << dist(mt);
  ss << "-";
  ss << dist2(mt);
  for (i = 0; i < 3; i++) ss << dist(mt);
  ss << "-";
  for (i = 0; i < 12; i++) ss << dist(mt);

  return ss.str();
}

std::string currentTimestamp() {
  auto now = std::chrono::system_clock::now();
  time_t t = std::chrono::system_clock::to_time_t(now);

  std::tm* tm = std::localtime(&t);
  std::ostringstream ss;

  ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
};

bool isValidEmail(std::string_view email) {
  const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
  /* Using an iterator because std::regex_match doesn't have overload
   * that accepts std::string_view */
  return std::regex_match(email.begin(), email.end(), pattern);
}

bool isDigitsOnly(std::string_view str) {
  return !str.empty() &&
         std::find_if(str.begin(), str.end(), [](unsigned char c) {
           return !std::isdigit(c);
         }) == str.end();
}

bool isValidPhone(std::string_view phone) {
  /* TODO: standardise phone display: add country-code prefix (e.g. +39)
   * and decide whether to keep phone as std::string or introduce a dedicated
   * type/format. Handle at end-of-project cleanup. */
  return isDigitsOnly(phone);
}

}  // namespace insura::domain::utils
