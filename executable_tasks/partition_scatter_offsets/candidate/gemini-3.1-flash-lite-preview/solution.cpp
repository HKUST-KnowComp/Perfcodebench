#include "interface.h"
#include <vector>
#include <numeric>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  const size_t n = keys.size();
  std::vector<uint32_t> counts(groups, 0);
  std::vector<uint32_t> offsets(groups + 1, 0);
  std::vector<uint32_t> rows(n);
  std::vector<uint32_t> current_pos(groups);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    for (uint32_t k : keys) {
      if (k < groups) {
        counts[k]++;
      }
    }

    offsets[0] = 0;
    for (uint32_t i = 0; i < groups; ++i) {
      offsets[i + 1] = offsets[i] + counts[i];
      current_pos[i] = offsets[i];
    }

    for (uint32_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < groups) {
        rows[current_pos[k]++] = i;
      }
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