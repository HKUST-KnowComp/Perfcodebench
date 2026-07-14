#include "interface.h"
#include <vector>

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
  uint32_t min_key = keys[0];
  uint32_t max_key = keys.back();
  size_t range = static_cast<size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> lookup(range);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i] - min_key] = values[i];
  }
  const uint32_t* lookup_ptr = lookup.data();
  const ptrdiff_t offset = static_cast<ptrdiff_t>(min_key);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      hash = mix(hash, lookup_ptr[probe - offset]);
    }
  }
  return hash;
}