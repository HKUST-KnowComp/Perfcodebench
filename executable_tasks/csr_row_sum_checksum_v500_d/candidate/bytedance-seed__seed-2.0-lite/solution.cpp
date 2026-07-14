#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const std::size_t num_rows = row_ptr.size() - 1;
  std::vector<uint64_t> row_sums(num_rows, 0);
  
  // Precompute row sums once to avoid redundant work
  for (std::size_t r = 0; r < num_rows; ++r) {
    const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    const std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
    uint64_t sum = 0;
    for (std::size_t j = start; j < end; ++j) {
      sum += static_cast<uint64_t>(values[j]);
    }
    row_sums[r] = sum;
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = 1469598103934665603ULL;
    for (const uint64_t sum : row_sums) {
      final_hash = mix(final_hash, sum);
    }
  }
  return final_hash;
}