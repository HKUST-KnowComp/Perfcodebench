#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const size_t nrows = row_ptr.size() - 1;
  std::vector<uint64_t> row_sums(nrows);
  for (size_t r = 0; r < nrows; ++r) {
    size_t start = static_cast<size_t>(row_ptr[r]);
    size_t end = static_cast<size_t>(row_ptr[r + 1]);
    uint64_t sum = 0;
    for (size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    row_sums[r] = sum;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < nrows; ++r) {
      hash = mix(hash, row_sums[r]);
    }
  }
  return hash;
}