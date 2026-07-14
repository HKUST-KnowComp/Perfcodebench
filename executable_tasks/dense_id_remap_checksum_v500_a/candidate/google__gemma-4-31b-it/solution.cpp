#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // Find the range of keys to build a lookup table
  uint32_t max_key = keys.back();
  std::vector<uint32_t> lookup(max_key + 1);

  // The baseline uses std::lower_bound, which finds the first element 
  // not less than the probe. Since keys are sorted, we can pre-calculate
  // the value associated with the lower_bound for every possible probe value.
  size_t current_key_idx = 0;
  for (uint32_t i = 0; i <= max_key; ++i) {
    while (current_key_idx < keys.size() && keys[current_key_idx] < i) {
      current_key_idx++;
    }
    // If i > max_key, lower_bound returns keys.end(), so idx = keys.size()
    // However, the loop only goes up to max_key. 
    // If current_key_idx == keys.size(), it means i > all keys.
    if (current_key_idx < keys.size()) {
      lookup[i] = values[current_key_idx];
    } else {
      // This case handles probes that are larger than any key in the keys vector
      // but we only fill the table up to max_key. 
      // The baseline would use values[keys.size()], which is out of bounds 
      // unless values.size() > keys.size(). 
      // Based on the baseline: idx = it - keys.begin(). 
      // If it == keys.end(), idx == keys.size().
      // We must ensure we don't crash. 
      lookup[i] = (values.size() > keys.size()) ? values[keys.size()] : 0;
    }
  }

  // Handle probes that might exceed max_key
  uint32_t overflow_val = (values.size() > keys.size()) ? values[keys.size()] : 0;

  uint64_t final_hash = 0;
  const uint32_t* probe_ptr = probes.data();
  size_t probe_count = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < probe_count; ++i) {
      uint32_t p = probe_ptr[i];
      uint32_t val = (p <= max_key) ? lookup[p] : overflow_val;
      h = mix(h, val);
    }
    final_hash = h;
  }

  return final_hash;
}