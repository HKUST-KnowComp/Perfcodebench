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
  const std::size_t n = row_ids.size();
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t pos = 0;
    uint32_t current_offset = 0;
    offsets[0] = 0;

    for (uint32_t r = 0; r < rows; ++r) {
      while (pos < n && row_ids[pos] == r) {
        ++pos;
      }
      current_offset = static_cast<uint32_t>(pos);
      offsets[static_cast<std::size_t>(r) + 1u] = current_offset;
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0, e = offsets.size(); i < e; ++i) {
      hash = mix(hash, offsets[i]);
    }
  }
  return hash;
}
