#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  const uint32_t* rp = row_ptr.data();
  const uint32_t* val = values.data();
  std::size_t nrows = row_ptr.size() - 1;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < nrows; ++r) {
      std::size_t start = static_cast<std::size_t>(rp[r]);
      std::size_t end = static_cast<std::size_t>(rp[r+1]);
      uint64_t sum = 0;
      for (std::size_t i = start; i < end; ++i) {
        sum += val[i];
      }
      hash = mix(hash, sum);
    }
  }
  return hash;
}
