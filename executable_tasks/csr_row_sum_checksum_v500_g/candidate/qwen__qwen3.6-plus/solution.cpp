#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const uint32_t* vp = values.data();
  const uint32_t* rp = row_ptr.data();
  size_t num_rows = row_ptr.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < num_rows; ++r) {
      uint64_t sum = 0;
      size_t start = rp[r];
      size_t end = rp[r + 1];
      for (size_t i = start; i < end; ++i) {
        sum += vp[i];
      }
      hash ^= sum;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
