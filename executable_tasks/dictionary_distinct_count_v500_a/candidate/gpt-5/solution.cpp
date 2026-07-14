#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // If no iterations requested or no codes, result is zero.
  if (iters <= 0 || codes.empty()) {
    return 0;
  }

  // Deduplicate code indices efficiently using a fixed-size visited array
  // based on uint16_t domain (0..65535).
  std::vector<uint8_t> visited(65536, 0);
  std::vector<uint16_t> uniqueCodes;
  uniqueCodes.reserve(codes.size());
  for (uint16_t c : codes) {
    if (!visited[c]) {
      visited[c] = 1;
      uniqueCodes.push_back(c);
    }
  }

  // Count distinct strings among the uniquely referenced dictionary entries.
  std::unordered_set<std::string> seen;
  seen.reserve(uniqueCodes.size());
  for (uint16_t c : uniqueCodes) {
    // Preserve baseline semantics: direct indexing, assuming codes are valid.
    seen.emplace(dictionary[static_cast<std::size_t>(c)]);
  }

  return static_cast<uint64_t>(seen.size());
}
