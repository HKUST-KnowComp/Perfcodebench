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

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows,
                          int iters) {
  // reusable buffers
  std::vector<uint32_t> hist(rows + 1, 0);
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // 1. zero histogram
    std::fill(hist.begin(), hist.end(), 0u);

    // 2. count row occurrences
    for (uint32_t r : row_ids) {
      ++hist[r];
    }

    // 3. in-place prefix sum -> CSR offsets
    uint32_t sum = 0;
    for (uint32_t r = 0; r <= rows; ++r) {
      uint32_t cnt = hist[r];
      hist[r] = sum;
      sum += cnt;
    }

    // 4. compute checksum while data is hot in cache
    if (iter == iters - 1) {          // only last iteration contributes to final hash
      hash = 1469598103934665603ULL;
      for (uint32_t v : hist) {
        hash = mix(hash, v);
      }
    }
  }
  return hash;
}