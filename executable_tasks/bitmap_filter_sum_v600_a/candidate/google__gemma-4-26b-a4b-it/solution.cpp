#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;

  // Use a bitset for O(1) membership testing.
  // We use a vector of uint64_t to represent the bitmap.
  size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words, 0);

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap for this iteration
    std::fill(bitmap.begin(), bitmap.end(), 0);

    // Populate bitmap
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        bitmap[key >> 6] |= (1ULL << (key & 63));
      }
    }

    uint64_t current_iter_sum = 0;
    size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();

    // Aggregate values
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = p_keys[i];
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