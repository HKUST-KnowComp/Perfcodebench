#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) return 0;
  
  const uint32_t* rp = row_ptr.data();
  const uint32_t* vp = values.data();
  size_t n_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < n_rows; ++r) {
      uint32_t start = rp[r];
      uint32_t end = rp[r+1];
      uint64_t sum = 0;
      const uint32_t* v = vp + start;
      const uint32_t* vend = vp + end;
      while (v != vend) {
        sum += *v++;
      }
      hash ^= sum;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
