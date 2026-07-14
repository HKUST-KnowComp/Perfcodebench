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
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < row_ptr.size(); ++r) {
      std::vector<uint32_t> row(
          values.begin() + static_cast<std::size_t>(row_ptr[r]),
          values.begin() + static_cast<std::size_t>(row_ptr[r + 1]));
      uint64_t sum = 0;
      for (uint32_t v : row) {
        sum += static_cast<uint64_t>(v);
      }
      hash = mix(hash, sum);
    }
  }
  return hash;
}
