#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

static inline bool is_upper(char c)   { return (static_cast<unsigned>(c - 'A') < 26U); }
static inline bool is_digit(char c) { return (static_cast<unsigned>(c - '0') < 10U); }
static inline bool is_lower(char c)   { return (static_cast<unsigned>(c - 'a') < 26U); }
static inline bool is_xdigit(char c) {
  return (static_cast<unsigned>(c - '0') < 10U) ||
         (static_cast<unsigned>((c & ~0x20) - 'A') < 6U);
}

// Returns true for strings of the form  ^[A-Z]{2}[0-9]{4}-[a-z]{6}-[0-9A-F]{8}$
// The pattern length is always 22.
static bool match_pattern(const std::string& s) noexcept {
  const std::size_t n = s.size();
  if (n != 22) return false;
  const char* p = s.data();

  // positions 0-1 : [A-Z]{2}
  if (!is_upper(p[0]) || !is_upper(p[1])) return false;
  // positions 2-5 : [0-9]{4}
  if (!is_digit(p[2]) || !is_digit(p[3]) || !is_digit(p[4]) || !is_digit(p[5])) return false;
  // position 6   : must be '-'
  if (p[6] != '-') return false;
  // positions 7-12: [a-z]{6}
  for (int i = 7; i < 13; ++i)
    if (!is_lower(p[i])) return false;
  // position 13  : must be '-'
  if (p[13] != '-') return false;
  // positions 14-21: [0-9A-F]{8}
  for (int i = 14; i < 22; ++i)
    if (!is_xdigit(p[i])) return false;

  return true;
}

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  constexpr uint64_t FNV_offset_basis = 1469598103934665603ULL;
  constexpr uint64_t FNV_prime = 1099511628211ULL;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_offset_basis;
    for (const std::string& s : records) {
      const bool ok = match_pattern(s);
      const uint64_t v = ok ? static_cast<uint64_t>(static_cast<unsigned char>(s[2]) +
                                                      static_cast<unsigned char>(s[14])) : 0u;
      hash ^= (v + 1u);
      hash *= FNV_prime;
    }
  }
  return hash;
}