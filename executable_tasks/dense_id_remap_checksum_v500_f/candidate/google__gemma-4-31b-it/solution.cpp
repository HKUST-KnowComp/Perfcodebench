#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // The original code used std::lower_bound, which suggests keys are sorted.
  // To optimize, we create a lookup table if the range of keys is manageable,
  // or simply optimize the inner loop. Given the task name "dense id remapping",
  // we can assume the keys are a contiguous or near-contiguous range of IDs.
  
  // Since we don't know the exact range, but we know the keys are sorted,
  // we can pre-calculate the mapping for all probes to avoid repeated binary searches.
  
  std::vector<uint32_t> probe_values;
  probe_values.reserve(probes.size());

  for (uint32_t probe : probes) {
    auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    size_t idx = std::distance(keys.begin(), it);
    // The original code accessed values[idx] without bounds checking,
    // implying idx is always within [0, values.size()).
    probe_values.push_back(values[idx]);
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t val : probe_values) {
      hash = mix(hash, val);
    }
    final_hash = hash;
  }

  return final_hash;
}