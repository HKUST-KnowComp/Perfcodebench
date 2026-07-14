#include "interface.h"
#include <vector>
#include <cstring>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // build perfect-hash remap table: remap[probe] = values[idx] or 0
  uint32_t max_key = 0;
  for (uint32_t k : keys) max_key = (k > max_key ? k : max_key);
  std::vector<uint32_t> remap(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) remap[keys[i]] = values[i];

  uint64_t hash = 14695981039346656037ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 14695981039346656037ULL;
    for (uint32_t probe : probes) {
      uint64_t v = remap[probe];
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}