#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Precompute the sum for each row into a vector of uint64_t.
  std::vector<uint64_t> row_sums;
  const std::size_t n_rows = row_ptr.size();
  if (n_rows > 1) {
    row_sums.reserve(n_rows - 1);
    for (std::size_t r = 0; r + 1 < n_rows; ++r) {
      std::size_t start = static_cast<std::size_t>(row_ptr[r]);
      std::size_t end   = static_cast<std::size_t>(row_ptr[r + 1]);
      uint64_t sum = 0;
      for (std::size_t i = start; i < end; ++i) {
        sum += static_cast<uint64_t>(values[i]);
      }
      row_sums.push_back(sum);
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t s : row_sums) {
      hash ^= s;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
