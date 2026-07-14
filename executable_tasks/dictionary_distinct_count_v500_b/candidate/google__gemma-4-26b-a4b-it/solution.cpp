#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) {
    return 0;
  }

  uint64_t last_count = 0;
  size_t dict_size = dictionary.size();
  
  // Pre-allocate a presence vector to avoid re-allocation.
  // Using a vector<bool> or a manual bitset is efficient for tracking seen indices.
  std::vector<bool> seen(dict_size, false);
  // We also need a way to track which indices were set to true to reset them efficiently.
  std::vector<uint16_t> changed_indices;
  changed_indices.reserve(codes.size());

  for (int iter = 0; iter < iters; ++iter) {
    changed_indices.clear();
    uint64_t current_distinct = 0;

    for (uint16_t code : codes) {
      // Safety check: ensure code is within dictionary bounds
      // (Though the problem implies codes are valid dictionary indices)
      if (static_cast<size_t>(code) < dict_size) {
        if (!seen[code]) {
          seen[code] = true;
          changed_indices.push_back(code);
          current_distinct++;
        }
      }
    }

    last_count = current_distinct;

    // Reset the 'seen' flags for the next iteration without zeroing the whole vector
    for (uint16_t idx : changed_indices) {
      seen[idx] = false;
    }
  }

  return last_count;
}