#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Precompute row sums
  std::size_t num_rows = row_ptr.size() - 1;
  std::vector<uint64_t> row_sums(num_rows);
  for (std::size_t r = 0; r < num_rows; ++r) {
    std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
    uint64_t sum = 0;
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    row_sums[r] = sum;
  }

  // Compute final hash over iters
  uint64_t hash = 0;
  const uint64_t* sums_data = row_sums.data();
  std::size_t sz = num_rows;
  const uint64_t init = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = init;
    for (std::size_t i = 0; i < sz; ++i) {
      h = mix(h, sums_data[i]);
    }
    hash = h;
  }
  return hash;
}
