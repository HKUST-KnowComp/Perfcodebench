#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Handle empty dimension case
  if (dim_keys.empty()) {
    return 0;
  }
  
  // Build direct lookup table for dense dimension
  // Find min and max key to create dense mapping
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] < min_key) min_key = dim_keys[i];
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }
  
  // Create direct lookup table with size = max_key - min_key + 1
  std::size_t table_size = static_cast<std::size_t>(max_key - min_key + 1);
  std::vector<uint32_t> lookup_table(table_size);
  
  // Populate lookup table: index = key - min_key
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup_table[dim_keys[i] - min_key] = dim_values[i];
  }
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t key = probe_keys[i];
      // Check if key is within valid range
      if (key >= min_key && key <= max_key) {
        std::size_t idx = static_cast<std::size_t>(key - min_key);
        sum += static_cast<uint64_t>(lookup_table[idx]) * 
               static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }
  return sum;
}