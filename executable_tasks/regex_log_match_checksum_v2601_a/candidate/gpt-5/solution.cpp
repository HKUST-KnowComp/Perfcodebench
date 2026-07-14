#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

static inline bool is_upper_AZ(char c) {
  return c >= 'A' && c <= 'Z';
}

static inline bool is_lower_az(char c) {
  return c >= 'a' && c <= 'z';
}

static inline bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool is_hex_upper(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  // Preserve baseline behavior: if iters == 0, return 0
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  for (const std::string& s : records) {
    bool ok = false;
    if (s.size() == 22 && s[6] == '-' && s[13] == '-') {
      // Check [A-Z]{2}
      if (is_upper_AZ(s[0]) && is_upper_AZ(s[1])) {
        // Check [0-9]{4}
        if (is_digit(s[2]) && is_digit(s[3]) && is_digit(s[4]) && is_digit(s[5])) {
          // Check [a-z]{6}
          if (is_lower_az(s[7]) && is_lower_az(s[8]) && is_lower_az(s[9]) &&
              is_lower_az(s[10]) && is_lower_az(s[11]) && is_lower_az(s[12])) {
            // Check [0-9A-F]{8}
            if (is_hex_upper(s[14]) && is_hex_upper(s[15]) && is_hex_upper(s[16]) && is_hex_upper(s[17]) &&
                is_hex_upper(s[18]) && is_hex_upper(s[19]) && is_hex_upper(s[20]) && is_hex_upper(s[21])) {
              ok = true;
            }
          }
        }
      }
    }

    uint64_t v = 0u;
    if (ok) {
      // Sum of characters at positions 2 and 14, as in baseline.
      v = static_cast<uint64_t>(static_cast<unsigned char>(s[2]) + static_cast<unsigned char>(s[14]));
    }
    hash ^= (v + 1u);
    hash *= FNV_PRIME;
  }

  return hash;
}
