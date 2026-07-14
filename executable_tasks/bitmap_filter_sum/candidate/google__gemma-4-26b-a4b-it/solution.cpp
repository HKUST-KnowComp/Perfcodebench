#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;

  // Use a bitset for O(1) lookup. 
  // Size is (key_space + 63) / 64 to accommodate all possible keys.
  size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words, 0);

  uint64_t total_sum = 0;
  size_t num_probes = probe_keys.size();n
  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap efficiently
    std::fill(bitmap.begin(), bitmap.end(), 0);

    // Populate bitmap
    for (uint32_t key : enabled_keys) {
      // Safety check for key_space bounds if necessary, 
      // though task implies keys are within [0, key_space)
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }

    uint64_t current_iter_sum = 0;
    // Process probes
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t key = probe_keys[i];
      // Check if bit is set
      if (bitmap[key >> 6] & (1ULL << (key & 63))) {
        current_iter_sum += static_cast<uint64_t>(values[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}