#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  std::vector<std::pair<uint32_t, uint32_t>> probe_pairs;
  probe_pairs.reserve(probe_keys.size());
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    probe_pairs.emplace_back(probe_keys[i], probe_weights[i]);
  }
  std::sort(probe_pairs.begin(), probe_pairs.end());

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_ptr = 0;
    for (const auto& pair : probe_pairs) {
      while (dim_keys[dim_ptr] < pair.first) {
        dim_ptr++;
      }
      sum += static_cast<uint64_t>(dim_values[dim_ptr]) * pair.second;
    }
  }
  return sum;
}