#include "interface.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Step 1: Map each dictionary index to a 'canonical' ID representing its string content.
  // This handles the case where dictionary[i] == dictionary[j] but i != j.
  std::vector<uint32_t> canonical_ids(dictionary.size());
  std::unordered_map<std::string, uint32_t> string_to_id;
  uint32_t next_id = 0;

  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    auto it = string_to_id.find(dictionary[i]);
    if (it == string_to_id.end()) {
      string_to_id[dictionary[i]] = next_id;
      canonical_ids[i] = next_id;
      next_id++;
    } else {
      canonical_ids[i] = it->second;
    }
  }

  // Step 2: Pre-map the codes to their canonical IDs to avoid dictionary lookups in the loop.
  std::vector<uint32_t> mapped_codes(codes.size());
  for (std::size_t i = 0; i < codes.size(); ++i) {
    mapped_codes[i] = canonical_ids[static_cast<std::size_t>(codes[i])];
  }

  // Step 3: Perform the iterations.
  // We use a boolean vector (or bitset) to track seen canonical IDs.
  // This is much faster than std::unordered_set<std::string>.
  std::vector<bool> seen(next_id, false);
  uint64_t last_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset seen array
    std::fill(seen.begin(), seen.end(), false);
    uint64_t current_distinct = 0;

    for (uint32_t id : mapped_codes) {
      if (!seen[id]) {
        seen[id] = true;
        current_distinct++;
      }
    }
    last_count = current_distinct;
  }

  return last_count;
}