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

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const std::size_t n = row_ids.size();
    std::size_t i = 0;
    uint32_t offset = 0;
    offsets[0] = 0;

    for (uint32_t r = 0; r < rows; ++r) {
      while (i < n && row_ids[i] == r) {
        ++i;
      }
      offset = static_cast<uint32_t>(i);
      offsets[static_cast<std::size_t>(r) + 1u] = offset;
    }

    hash = 1469598103934665603ULL;
    for (std::size_t k = 0, e = offsets.size(); k < e; ++k) {
      hash = mix(hash, offsets[k]);
    }
  }

  return hash;
}
