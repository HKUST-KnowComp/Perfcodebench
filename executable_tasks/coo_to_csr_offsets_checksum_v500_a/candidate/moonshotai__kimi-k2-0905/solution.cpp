#include "interface.h"
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> hist(rows, 0);
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // build histogram
    std::fill(hist.begin(), hist.end(), 0);
    for (uint32_t r : row_ids) {
      ++hist[r];
    }

    // prefix sum into offsets
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + hist[r];
    }

    // checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}