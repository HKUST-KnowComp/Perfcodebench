#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline bool is_upper_ascii(unsigned char c) {
  return static_cast<unsigned>(c - 'A') <= static_cast<unsigned>('Z' - 'A');
}

inline bool is_lower_ascii(unsigned char c) {
  return static_cast<unsigned>(c - 'a') <= static_cast<unsigned>('z' - 'a');
}

inline bool is_digit_ascii(unsigned char c) {
  return static_cast<unsigned>(c - '0') <= 9u;
}

inline bool is_hex_upper_ascii(unsigned char c) {
  return is_digit_ascii(c) || static_cast<unsigned>(c - 'A') <= static_cast<unsigned>('F' - 'A');
}

inline bool match_log_id(const std::string& s) {
  if (s.size() != 17) return false;

  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());

  return is_upper_ascii(p[0]) &&
         is_upper_ascii(p[1]) &&
         is_digit_ascii(p[2]) &&
         is_digit_ascii(p[3]) &&
         is_digit_ascii(p[4]) &&
         is_digit_ascii(p[5]) &&
         p[6] == '-' &&
         is_lower_ascii(p[7]) &&
         is_lower_ascii(p[8]) &&
         is_lower_ascii(p[9]) &&
         is_lower_ascii(p[10]) &&
         is_lower_ascii(p[11]) &&
         is_lower_ascii(p[12]) &&
         p[13] == '-' &&
         is_hex_upper_ascii(p[14]) &&
         is_hex_upper_ascii(p[15]) &&
         is_hex_upper_ascii(p[16]) &&
         is_hex_upper_ascii(p[17 - 1]);
}

}  // namespace

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const std::string& s : records) {
      bool ok = false;
      uint64_t v = 0;
      if (s.size() == 17) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
        ok = is_upper_ascii(p[0]) &&
             is_upper_ascii(p[1]) &&
             is_digit_ascii(p[2]) &&
             is_digit_ascii(p[3]) &&
             is_digit_ascii(p[4]) &&
             is_digit_ascii(p[5]) &&
             p[6] == '-' &&
             is_lower_ascii(p[7]) &&
             is_lower_ascii(p[8]) &&
             is_lower_ascii(p[9]) &&
             is_lower_ascii(p[10]) &&
             is_lower_ascii(p[11]) &&
             is_lower_ascii(p[12]) &&
             p[13] == '-' &&
             is_hex_upper_ascii(p[14]) &&
             is_hex_upper_ascii(p[15]) &&
             is_hex_upper_ascii(p[16]);
        if (ok) v = static_cast<uint64_t>(p[2] + p[14]);
      }
      hash ^= v + 1u;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
