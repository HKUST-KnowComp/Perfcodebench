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
  if (values.size() != n || iters <= 0) {
    return 0;
  }

  // Dense accumulator is substantially faster than a hash table for bounded
  // domains and also makes the final checksum pass cache-friendly.
  std::vector<uint64_t> accum(domain, 0ULL);
  std::vector<uint32_t> touched;
  touched.reserve(n < static_cast<std::size_t>(domain) ? n : static_cast<std::size_t>(domain));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t idx = indices[i];
      const uint64_t v = static_cast<uint64_t>(values[i]);
      if (accum[idx] == 0ULL) {
        touched.push_back(idx);
      }
      accum[idx] += v;
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      h = mix(h, accum[key]);
    }
    hash = h;

    for (uint32_t idx : touched) {
      accum[idx] = 0ULL;
    }
    touched.clear();
  }

  return hash;
}
