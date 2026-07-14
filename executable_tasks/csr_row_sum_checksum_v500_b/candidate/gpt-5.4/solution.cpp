#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
  const std::size_t rows = row_ptr.size() > 0 ? (row_ptr.size() - 1) : 0;
  const uint32_t* const rp = row_ptr.data();
  const uint32_t* const vals = values.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (std::size_t r = 0; r < rows; ++r) {
      const uint32_t start = rp[r];
      const uint32_t end = rp[r + 1];
      const uint32_t* p = vals + start;
      const uint32_t* const pe = vals + end;

      uint64_t sum = 0;

      while (p + 4 <= pe) {
        sum += static_cast<uint64_t>(p[0]);
        sum += static_cast<uint64_t>(p[1]);
        sum += static_cast<uint64_t>(p[2]);
        sum += static_cast<uint64_t>(p[3]);
        p += 4;
      }
      while (p < pe) {
        sum += static_cast<uint64_t>(*p++);
      }

      hash = mix(hash, sum);
    }
  }
  return hash;
}
