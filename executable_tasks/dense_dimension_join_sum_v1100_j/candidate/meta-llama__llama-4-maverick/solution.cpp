#include "interface.h"

#include <unordered_map>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  std::unordered_map<uint32_t, uint32_t> dim_map;
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    dim_map[dim_keys[i]] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      auto it = dim_map.find(probe_keys[i]);
      if (it != dim_map.end()) {
        sum += static_cast<uint64_t>(it->second) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }
  return sum;
}