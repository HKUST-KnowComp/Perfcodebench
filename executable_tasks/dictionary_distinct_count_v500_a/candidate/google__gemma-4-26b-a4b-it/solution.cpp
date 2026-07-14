#include "interface.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Step 1: Map each dictionary index to a unique ID representing its string content.
  // This handles the case where dictionary[i] == dictionary[j].
  size_t dict_size = dictionary.size();
  std::vector<uint32_t> canonical_map(dict_size);
  std::unordered_map<std::string, uint32_t> string_to_id;
  uint32_t next_id = 0;

  for (size_t i = 0; i < dict_size; ++i) {
    auto it = string_to_id.find(dictionary[i]);
    if (it == string_to_id.end()) {
      string_to_id[dictionary[i]] = next_id;
      canonical_map[i] = next_id;
      next_id++;
    } else {
      canonical_map[i] = it->second;
    }
  }

  // Step 2: Pre-convert codes to their canonical IDs to minimize work in the loop.
  std::vector<uint32_t> canonical_codes(codes.size());
  for (size_t i = 0; i < codes.size(); ++i) {
    canonical_codes[i] = canonical_map[static_cast<size_t>(codes[i])];
  }

  uint64_t final_count = 0;
  // We use a vector of bool (or a bitset-like structure) to track seen canonical IDs.
  // Since next_id is at most dict_size, this is efficient.
  std::vector<bool> seen(next_id);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), false);
    uint32_t current_distinct = 0;
    
    for (uint32_t cid : canonical_codes) {
      if (!seen[cid]) {
        seen[cid] = true;
        current_distinct++;
      }
    }
    final_count = static_cast<uint64_t>(current_distinct);
  }

  return final_count;
}