#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const std::size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  const uint32_t* const val_data = values.data();
  const uint32_t* const rp_data = row_ptr.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_rows; ++r) {
      const uint32_t start = rp_data[r];
      const uint32_t end = rp_data[r + 1];
      uint64_t sum = 0;
      for (uint32_t i = start; i < end; ++i) {
        sum += val_data[i];
      }
      hash ^= sum;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}