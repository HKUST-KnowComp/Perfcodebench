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
  // Size is (key_space + 63) / 64 uint64_t elements.
  size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words, 0);

  // Pre-calculate the bitset once if enabled_keys doesn't change per iteration.
  // Based on the baseline, enabled_keys is constant across 'iters'.
  for (uint32_t key : enabled_keys) {
    if (key < key_space) {
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_vals = values.data();

  // The baseline re-inserts enabled_keys every iteration, but since enabled_keys
  // is constant, we can move the bitset construction outside the loop.
  // However, to strictly follow the baseline's logic of 'sum = 0' inside the loop:
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = p_keys[i];
      if (k < key_space) {
        if (bitmap[k >> 6] & (1ULL << (k & 63))) {
          current_iter_sum += static_cast<uint64_t>(p_vals[i]);
        }
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}