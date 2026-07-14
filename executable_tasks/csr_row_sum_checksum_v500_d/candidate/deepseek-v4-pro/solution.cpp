#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Precompute per-row sums once.
  const std::size_t nrows = (row_ptr.size() > 0) ? row_ptr.size() - 1 : 0;
  std::vector<uint64_t> row_sums(nrows);

  const uint32_t* ptr_data = row_ptr.data();
  const uint32_t* val_data = values.data();
  uint64_t* sum_data = row_sums.data();

  for (std::size_t r = 0; r < nrows; ++r) {
    uint64_t sum = 0;
    const std::size_t start = static_cast<std::size_t>(ptr_data[r]);
    const std::size_t end   = static_cast<std::size_t>(ptr_data[r + 1]);
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(val_data[i]);
    }
    sum_data[r] = sum;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < nrows; ++r) {
      hash = mix(hash, sum_data[r]);
    }
  }
  return hash;
}
