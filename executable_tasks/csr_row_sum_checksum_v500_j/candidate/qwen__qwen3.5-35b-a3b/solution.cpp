#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const uint32_t* values_ptr = values.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < row_ptr.size(); ++r) {
      uint64_t sum = 0;
      const uint32_t* begin = values_ptr + row_ptr[r];
      const uint32_t* end = values_ptr + row_ptr[r + 1];
      for (const uint32_t* p = begin; p != end; ++p) {
        sum += *p;
      }
      hash = mix(hash, sum);
    }
  }
  return hash;
}