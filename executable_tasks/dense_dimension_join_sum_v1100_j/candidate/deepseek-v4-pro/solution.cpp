#include "interface.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  // Build fast O(1) lookup table for the dense dimension
  uint32_t min_key = std::numeric_limits<uint32_t>::max();
  uint32_t max_key = 0;
  for (uint32_t k : dim_keys) {
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  // Flat array covering the entire key range
  const std::size_t range = static_cast<std::size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> flat(range, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    flat[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      const uint32_t val = flat[probe_keys[i] - min_key];
      sum += static_cast<uint64_t>(val) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}