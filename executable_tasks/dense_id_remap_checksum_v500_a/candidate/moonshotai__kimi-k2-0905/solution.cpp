#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix8(uint64_t h0, uint64_t h1, uint64_t h2, uint64_t h3,
                     uint64_t h4, uint64_t h5, uint64_t h6, uint64_t h7) {
  h0 ^= h1;  h0 *= 1099511628211ULL;
  h0 ^= h2;  h0 *= 1099511628211ULL;
  h0 ^= h3;  h0 *= 1099511628211ULL;
  h0 ^= h4;  h0 *= 1099511628211ULL;
  h0 ^= h5;  h0 *= 1099511628211ULL;
  h0 ^= h6;  h0 *= 1099511628211ULL;
  h0 ^= h7;  h0 *= 1099511628211ULL;
  return h0;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // Build direct-index table keyed by id
  uint32_t max_key = *std::max_element(keys.begin(), keys.end());
  std::vector<uint32_t> remap(max_key + 1);
  for (size_t i = 0; i < keys.size(); ++i) {
    remap[keys[i]] = values[i];
  }

  // Pre-compute the remapped values for all probes
  std::vector<uint32_t> remapped_probes(probes.size());
  for (size_t i = 0; i < probes.size(); ++i) {
    uint32_t id = probes[i];
    remapped_probes[i] = (id <= max_key) ? remap[id] : 0;
  }

  uint64_t hash = 0;
  const size_t n = remapped_probes.size();
  const size_t n8 = n & ~size_t(7);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0;

    // Vectorized loop: process 8 values at once
    for (; i < n8; i += 8) {
      uint64_t h0 = hash ^ remapped_probes[i + 0];
      uint64_t h1 = hash ^ remapped_probes[i + 1];
      uint64_t h2 = hash ^ remapped_probes[i + 2];
      uint64_t h3 = hash ^ remapped_probes[i + 3];
      uint64_t h4 = hash ^ remapped_probes[i + 4];
      uint64_t h5 = hash ^ remapped_probes[i + 5];
      uint64_t h6 = hash ^ remapped_probes[i + 6];
      uint64_t h7 = hash ^ remapped_probes[i + 7];

      h0 *= 1099511628211ULL;
      h1 *= 1099511628211ULL;
      h2 *= 1099511628211ULL;
      h3 *= 1099511628211ULL;
      h4 *= 1099511628211ULL;
      h5 *= 1099511628211ULL;
      h6 *= 1099511628211ULL;
      h7 *= 1099511628211ULL;

      hash = mix8(h0, h1, h2, h3, h4, h5, h6, h7);
    }

    // Tail
    for (; i < n; ++i) {
      hash ^= remapped_probes[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}