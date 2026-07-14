#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) return 0;
  
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* val_data = values.data();
  std::size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
  
  for (std::size_t r = 0; r < num_rows; ++r) {
    uint32_t start = row_ptr[r];
    uint32_t end = row_ptr[r + 1];
    uint64_t sum = 0;
    for (uint32_t i = start; i < end; ++i) {
      sum += val_data[i];
    }
    hash ^= sum;
    hash *= 1099511628211ULL;
  }
  return hash;
}