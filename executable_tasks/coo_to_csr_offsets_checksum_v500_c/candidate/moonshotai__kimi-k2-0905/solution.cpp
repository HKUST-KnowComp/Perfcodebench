#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t hash = 14695981039346656037ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // zero the offset array
    std::memset(offsets.data(), 0, (rows + 1) * sizeof(uint32_t));

    // count row lengths
    for (uint32_t row : row_ids) {
      ++offsets[row + 1];
    }

    // inplace prefix sum
    uint32_t sum = 0;
    for (size_t r = 1; r <= rows; ++r) {
      sum += offsets[r];
      offsets[r] = sum;
    }

    // compute checksum
    hash = 14695981039346656037ULL;
    for (size_t i = 0; i <= rows; ++i) {
      hash = mix(hash, offsets[i]);
    }
  }
  return hash;
}