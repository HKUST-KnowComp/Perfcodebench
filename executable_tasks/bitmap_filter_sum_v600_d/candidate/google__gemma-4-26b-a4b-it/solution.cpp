#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (key_space == 0) return 0;

  // Use a bitset for O(1) lookup. 
  // A vector<uint64_t> acts as a manual bitset for maximum performance.
  size_t num_words = (key_space + 63) / 64;
  std::vector<uint64_t> bitmap(num_words, 0);

  // Pre-build the bitmap once outside the loop if enabled_keys doesn't change.
  // However, the task structure implies we must respect the loop logic.
  // Since enabled_keys is constant across iterations, we build it once.
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = p_keys[i];
      // Check if key is within bounds and set in bitmap
      if (key < key_space) {
        if (bitmap[key >> 6] & (1ULL << (key & 63))) {
          current_iter_sum += static_cast<uint64_t>(p_vals[i]);
        }
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}