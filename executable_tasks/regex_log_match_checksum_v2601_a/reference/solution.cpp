#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline bool is_upper(char c) { return c >= 'A' && c <= 'Z'; }
inline bool is_lower(char c) { return c >= 'a' && c <= 'z'; }
inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
inline bool is_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

inline bool matches(const std::string& s) {
  if (s.size() != 22u) return false;
  if (!is_upper(s[0]) || !is_upper(s[1])) return false;
  if (!is_digit(s[2]) || !is_digit(s[3]) || !is_digit(s[4]) || !is_digit(s[5])) return false;
  if (s[6] != '-') return false;
  for (int i = 7; i < 13; ++i) if (!is_lower(s[static_cast<std::size_t>(i)])) return false;
  if (s[13] != '-') return false;
  for (int i = 14; i < 22; ++i) if (!is_hex(s[static_cast<std::size_t>(i)])) return false;
  return true;
}

}  // namespace

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const std::string& s : records) {
      const uint64_t v = matches(s) ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
      hash ^= v + 1u;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
