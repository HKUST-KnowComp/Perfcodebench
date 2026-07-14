#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Optimized search for the first element >= probe
inline size_t find_index(const uint32_t* keys, size_t size, uint32_t probe) {
  size_t low = 0;
  size_t high = size;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (keys[mid] < probe) {
      low = mid + 1;
    } else {
      high = mid;
    }
  }
  return low;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) return 0;
  if (keys.empty()) return 0;

  const size_t n_keys = keys.size();
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  const uint32_t first_key = keys_ptr[0];
  const uint32_t last_key = keys_ptr[n_keys - 1];

  // Check if the mapping is perfectly dense (contiguous range)
  bool is_dense = (last_key - first_key == (uint32_t)(n_keys - 1));

  uint64_t hash = 0;
  
  // The benchmark expects 'iters' iterations of work.
  // We optimize the inner loop based on the density of the keys.
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    if (is_dense) {
      for (uint32_t probe : probes) {
        size_t idx;
        if (probe <= first_key) {
          idx = 0;
        } else if (probe > last_key) {
          idx = n_keys; // This matches std::lower_bound behavior (returns end())
        } else {
          idx = static_cast<size_t>(probe - first_key);
        }
        
        // Note: std::lower_bound returns keys.end() if no element is >= probe.
        // The baseline uses values[idx], which would be out of bounds if idx == keys.size().
        // We must preserve the baseline's behavior (even if it risks OOB) for correctness.
        hash = mix(hash, values_ptr[idx]);
      }
    } else {
      // Fallback to optimized binary search if not dense
      for (uint32_t probe : probes) {
        size_t idx = find_index(keys_ptr, n_keys, probe);
        hash = mix(hash, values_ptr[idx]);
      }
    }
  }

  return hash;
}
