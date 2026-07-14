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

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
  // Precompute all row sums outside the iteration loop
  std::vector<uint64_t> row_sums;
  const std::size_t n_rows = (row_ptr.empty() ? 0 : row_ptr.size() - 1);
  row_sums.reserve(n_rows);
  for (std::size_t r = 0; r < n_rows; ++r) {
    std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    std::size_t end   = static_cast<std::size_t>(row_ptr[r + 1]);
    uint64_t sum = 0;
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    row_sums.push_back(sum);
  }

  uint64_t hash = 0;
  const uint64_t base = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = base;
    for (const uint64_t s : row_sums) {
      hash = mix(hash, s);
    }
  }
  return hash;
}