#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  const std::size_t row_count = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  const uint32_t* const rp = row_ptr.data();
  const uint32_t* const vals = values.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffsetBasis;
    for (std::size_t r = 0; r < row_count; ++r) {
      const uint32_t start = rp[r];
      const uint32_t end = rp[r + 1];

      uint64_t sum = 0;
      const uint32_t* p = vals + start;
      const uint32_t* const pe = vals + end;
      for (; p != pe; ++p) {
        sum += static_cast<uint64_t>(*p);
      }

      hash ^= sum;
      hash *= kPrime;
    }
  }
  return hash;
}
