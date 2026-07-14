#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  std::vector<std::size_t> indices(probe_keys.size());
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    indices[i] = i;
  }
  std::sort(indices.begin(), indices.end(), [&](std::size_t a, std::size_t b) {
    return probe_keys[a] < probe_keys[b];
  });

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    for (std::size_t i : indices) {
      uint32_t key = probe_keys[i];
      uint32_t weight = probe_weights[i];
      while (dim_idx < dim_keys.size() && dim_keys[dim_idx] < key) {
        ++dim_idx;
      }
      if (dim_idx < dim_keys.size() && dim_keys[dim_idx] == key) {
        sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(weight);
      }
    }
  }
  return sum;
}