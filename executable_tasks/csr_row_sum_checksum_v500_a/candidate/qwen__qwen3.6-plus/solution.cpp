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

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const uint32_t* rp = row_ptr.data();
  const uint32_t* vp = values.data();
  size_t n = row_ptr.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < n; ++r) {
      uint32_t start = rp[r];
      uint32_t end = rp[r+1];
      uint64_t sum = 0;
      for (uint32_t i = start; i < end; ++i) {
        sum += vp[i];
      }
      hash = mix(hash, sum);
    }
  }
  return hash;
}