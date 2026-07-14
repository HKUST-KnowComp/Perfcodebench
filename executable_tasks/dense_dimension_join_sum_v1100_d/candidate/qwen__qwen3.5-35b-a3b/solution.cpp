#include "interface.h"

#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  // Pre-process dimension keys into a direct lookup table
  uint32_t base = dim_keys[0];
  std::vector<uint32_t> lookup(dim_keys.size());
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[i] = dim_values[i];
  }

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t key = probe_keys[i];
      std::size_t idx = key - base;
      sum += static_cast<uint64_t>(lookup[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}