#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
  const uint64_t kInit = 1469598103934665603ULL;
  const std::size_t num_rows = row_ptr.size() - 1;
  uint64_t hash = kInit;

  for (int iter = 0; iter < iters; ++iter) {
    hash = kInit;
    for (std::size_t r = 0; r < num_rows; ++r) {
      const uint32_t* beg = &values[row_ptr[r]];
      const uint32_t* end = &values[row_ptr[r + 1]];
      uint64_t sum = 0;

      // 4-way unrolled summation
      const std::size_t len = end - beg;
      std::size_t i = 0;
      for (; i + 3 < len; i += 4) {
        sum += static_cast<uint64_t>(beg[i]);
        sum += static_cast<uint64_t>(beg[i + 1]);
        sum += static_cast<uint64_t>(beg[i + 2]);
        sum += static_cast<uint64_t>(beg[i + 3]);
      }
      for (; i < len; ++i) {
        sum += static_cast<uint64_t>(beg[i]);
      }

      hash = mix(hash, sum);
    }
  }
  return hash;
}