#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  const std::size_t n = indices.size();
  if (iters <= 0) return 0;

  // Dense bounded-domain storage is much faster than unordered_map for both
  // accumulation and final checksum traversal.
  std::vector<uint64_t> accum(domain, 0);
  std::vector<uint32_t> stamp(domain, 0);
  std::vector<uint32_t> touched;
  touched.reserve(n < static_cast<std::size_t>(domain) ? n : static_cast<std::size_t>(domain));

  uint64_t hash = 0;
  uint32_t gen = 1;

  for (int iter = 0; iter < iters; ++iter) {
    if (gen == 0) {
      // Rare wraparound case after 2^32-1 iterations.
      for (uint32_t idx : touched) stamp[idx] = 0;
      touched.clear();
      gen = 1;
    }

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t idx = indices[i];
      if (stamp[idx] != gen) {
        stamp[idx] = gen;
        accum[idx] = static_cast<uint64_t>(values[i]);
        touched.push_back(idx);
      } else {
        accum[idx] += static_cast<uint64_t>(values[i]);
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      const uint64_t v = (stamp[key] == gen) ? accum[key] : 0ULL;
      hash = mix(hash, v);
    }

    ++gen;
    touched.clear();
  }

  return hash;
}
