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

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  const std::size_t n = keys.size();
  const std::size_t gsz = static_cast<std::size_t>(groups);

  std::vector<uint32_t> counts(gsz, 0);
  std::vector<uint32_t> offsets(gsz + 1, 0);
  std::vector<uint32_t> next(gsz, 0);
  std::vector<uint32_t> rows(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t g = 0; g < gsz; ++g) {
      counts[g] = 0;
    }

    for (std::size_t i = 0; i < n; ++i) {
      ++counts[keys[i]];
    }

    offsets[0] = 0;
    for (std::size_t g = 0; g < gsz; ++g) {
      offsets[g + 1] = offsets[g] + counts[g];
      next[g] = offsets[g];
    }

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t k = keys[i];
      rows[next[k]++] = static_cast<uint32_t>(i);
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    for (uint32_t v : rows) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
