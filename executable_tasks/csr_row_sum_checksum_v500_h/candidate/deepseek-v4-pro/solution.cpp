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

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const std::size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
  std::vector<uint64_t> row_sums;
  row_sums.reserve(num_rows);
  for (std::size_t r = 0; r < num_rows; ++r) {
    uint64_t sum = 0;
    const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    const std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    row_sums.push_back(sum);
  }

  uint64_t hash = 0;
  const uint64_t init_hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = init_hash;
    for (std::size_t r = 0; r < num_rows; ++r) {
      hash = mix(hash, row_sums[r]);
    }
  }
  return hash;
}
