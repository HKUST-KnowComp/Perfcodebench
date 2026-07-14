#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const uint32_t* data = row_ids.data();
  const size_t n = row_ids.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    hash = mix(hash, 0);  // offset[0]

    const uint32_t* p = data;
    const uint32_t* const end = data + n;
    uint32_t current = 0;

    for (uint32_t r = 0; r < rows; ++r) {
      if (p == end) {
        // remaining rows are all zeros
        for (uint32_t i = 0; i < (rows - r); ++i) {
          hash = mix(hash, current);
        }
        break;
      }

      uint32_t count = 0;
      while (p != end && *p == r) {
        ++count;
        ++p;
      }
      current += count;
      hash = mix(hash, current);
    }
  }

  return hash;
}
