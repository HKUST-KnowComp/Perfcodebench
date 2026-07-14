#include "interface.h"

#include <cstdint>
#include <regex>
#include <string>
#include <vector>

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
  const std::regex pat("^[A-Z]{2}[0-9]{4}-[a-z]{6}-[0-9A-F]{8}$");
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const std::string& s : records) {
      const bool ok = std::regex_match(s, pat);
      const uint64_t v = ok ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
      hash ^= v + 1u;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
