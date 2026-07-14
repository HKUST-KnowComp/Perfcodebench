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
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // zero out the histogram (rows+1 already zeroed)
    std::fill(offsets.begin(), offsets.end(), 0u);

    // single-pass histogram
    const uint32_t* ptr = row_ids.data();
    const uint32_t* end = ptr + row_ids.size();
    for (; ptr != end; ++ptr) {
      ++offsets[*ptr];
    }

    // in-place prefix sum (CSR offsets)
    uint32_t acc = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      uint32_t tmp = offsets[r];
      offsets[r] = acc;
      acc += tmp;
    }
    offsets[rows] = acc;

    // checksum
    hash = 1469598103934665603ULL;
    const uint32_t* off_ptr = offsets.data();
    const uint32_t* off_end = off_ptr + (rows + 1);
    for (; off_ptr != off_end; ++off_ptr) {
      hash = mix(hash, *off_ptr);
    }
  }
  return hash;
}