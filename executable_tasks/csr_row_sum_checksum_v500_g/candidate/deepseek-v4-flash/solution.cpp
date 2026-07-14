#include "interface.h"

#include <vector>
#include <cstddef>

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
  const std::size_t num_rows = (row_ptr.empty() ? 0 : row_ptr.size() - 1);
  
  // Precompute all row sums once
  std::vector<uint64_t> row_sums(num_rows);
  const uint32_t* row_ptr_data = row_ptr.data();
  const uint32_t* values_data = values.data();
  
  for (std::size_t r = 0; r < num_rows; ++r) {
    std::size_t start = static_cast<std::size_t>(row_ptr_data[r]);
    std::size_t end   = static_cast<std::size_t>(row_ptr_data[r + 1]);
    uint64_t sum = 0;
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values_data[i]);
    }
    row_sums[r] = sum;
  }

  // Repeated hashing of the same row sums
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV-1a offset basis
    for (std::size_t r = 0; r < num_rows; ++r) {
      hash = mix(hash, row_sums[r]);
    }
  }
  return hash;
}
