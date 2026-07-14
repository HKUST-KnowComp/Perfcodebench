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
    // histogram pass: count occurrences per row
    for (uint32_t row : row_ids) {
      ++offsets[row];
    }
    // prefix sum in-place to build offsets
    uint32_t sum = 0;
    for (size_t r = 0; r <= rows; ++r) {
      uint32_t cnt = offsets[r];
      offsets[r] = sum;
      sum += cnt;
    }
    // compute checksum
    hash = 14695981039346656037ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}