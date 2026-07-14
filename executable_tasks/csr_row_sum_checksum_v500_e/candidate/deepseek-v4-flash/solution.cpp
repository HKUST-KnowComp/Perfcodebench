#include "interface.h"

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const std::size_t rows = row_ptr.size() - 1;
  const uint32_t* row_ptr_data = row_ptr.data();
  const uint32_t* values_data = values.data();
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < rows; ++r) {
      std::size_t start = static_cast<std::size_t>(row_ptr_data[r]);
      std::size_t end = static_cast<std::size_t>(row_ptr_data[r + 1]);
      uint64_t sum = 0;
      for (std::size_t i = start; i < end; ++i) {
        sum += static_cast<uint64_t>(values_data[i]);
      }
      hash ^= sum;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}