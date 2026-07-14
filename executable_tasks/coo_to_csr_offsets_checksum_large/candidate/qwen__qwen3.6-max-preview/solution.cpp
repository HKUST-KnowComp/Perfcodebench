#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 0;
  uint32_t n = static_cast<uint32_t>(row_ids.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    uint32_t idx = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r] = idx;
      hash = mix(hash, idx);
      while (idx < n && row_ids[idx] == r) {
        ++idx;
      }
    }
    offsets[rows] = idx;
    hash = mix(hash, idx);
  }
  return hash;
}
