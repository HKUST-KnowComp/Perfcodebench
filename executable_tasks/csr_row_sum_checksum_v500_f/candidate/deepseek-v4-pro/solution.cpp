#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  (void)iters;
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 1; r < row_ptr.size(); ++r) {
    uint64_t sum = 0;
    std::size_t beg = row_ptr[r - 1];
    std::size_t end = row_ptr[r];
    for (std::size_t i = beg; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    hash = mix(hash, sum);
  }
  return hash;
}