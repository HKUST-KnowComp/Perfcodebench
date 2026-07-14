#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const uint32_t* rp = row_ptr.data();
  const uint32_t* val = values.data();
  size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_rows; ++r) {
      uint64_t sum = 0;
      uint32_t start = rp[r];
      uint32_t end = rp[r + 1];
      for (uint32_t i = start; i < end; ++i) {
        sum += val[i];
      }
      hash ^= sum;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
