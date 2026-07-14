#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

static inline bool is_upper(char c) {
  return c >= 'A' && c <= 'Z';
}

static inline bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool is_lower(char c) {
  return c >= 'a' && c <= 'z';
}

static inline bool is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static inline bool matches_pattern(const std::string& s) {
  if (s.size() != 23) return false;
  
  // [A-Z]{2}
  if (!is_upper(s[0]) || !is_upper(s[1])) return false;
  
  // [0-9]{4}
  if (!is_digit(s[2]) || !is_digit(s[3]) || !is_digit(s[4]) || !is_digit(s[5])) return false;
  
  // -
  if (s[6] != '-') return false;
  
  // [a-z]{6}
  if (!is_lower(s[7]) || !is_lower(s[8]) || !is_lower(s[9]) || 
      !is_lower(s[10]) || !is_lower(s[11]) || !is_lower(s[12])) return false;
  
  // -
  if (s[13] != '-') return false;
  
  // [0-9A-F]{8}
  if (!is_hex_digit(s[14]) || !is_hex_digit(s[15]) || !is_hex_digit(s[16]) || !is_hex_digit(s[17]) ||
      !is_hex_digit(s[18]) || !is_hex_digit(s[19]) || !is_hex_digit(s[20]) || !is_hex_digit(s[21])) return false;
  
  return true;
}

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const std::string& s : records) {
      const bool ok = matches_pattern(s);
      const uint64_t v = ok ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
      hash ^= v + 1u;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}