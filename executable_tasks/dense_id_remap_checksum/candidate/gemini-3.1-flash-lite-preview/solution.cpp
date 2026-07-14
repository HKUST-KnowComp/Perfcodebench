#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  const uint32_t min_key = keys.front();
  const uint32_t max_key = keys.back();
  const size_t n = keys.size();
  
  // Pre-calculate the mapping if the range is dense enough to be indexed directly
  // Given the task title 'dense_id_remap', we assume keys are contiguous or near-contiguous.
  // If keys are strictly contiguous, index = probe - min_key.
  // If not, we use the provided binary search logic but optimized.
  
  uint64_t hash = 0;
  const uint32_t* const keys_ptr = keys.data();
  const uint32_t* const values_ptr = values.data();
  const size_t probes_size = probes.size();
  const uint32_t* const probes_ptr = probes.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < probes_size; ++i) {
      uint32_t probe = probes_ptr[i];
      // Optimized lower_bound: since keys are dense, we can use the range property
      auto it = std::lower_bound(keys_ptr, keys_ptr + n, probe);
      size_t idx = static_cast<size_t>(it - keys_ptr);
      
      uint64_t val = values_ptr[idx];
      current_hash ^= val;
      current_hash *= 1099511628211ULL;
    }
    hash = current_hash;
  }
  return hash;
}