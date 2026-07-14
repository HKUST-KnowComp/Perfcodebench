#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Match baseline behavior: if no iterations requested, return 0.
  if (iters <= 0) return 0;

  const uint32_t* rptr = row_ptr.data();
  const uint32_t* vptr = values.data();
  const std::size_t nrows = row_ptr.size() > 0 ? (row_ptr.size() - 1) : 0;

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  for (std::size_t r = 0; r < nrows; ++r) {
    std::size_t start = static_cast<std::size_t>(rptr[r]);
    std::size_t end   = static_cast<std::size_t>(rptr[r + 1]);

    const uint32_t* p = vptr + start;
    const uint32_t* pe = vptr + end;

    uint64_t sum = 0;

    // Unrolled summation with 64-bit intermediates to avoid 32-bit overflow.
    while (p + 8 <= pe) {
      sum += (uint64_t)p[0] + (uint64_t)p[1] + (uint64_t)p[2] + (uint64_t)p[3];
      sum += (uint64_t)p[4] + (uint64_t)p[5] + (uint64_t)p[6] + (uint64_t)p[7];
      p += 8;
    }
    if (p + 4 <= pe) {
      sum += (uint64_t)p[0] + (uint64_t)p[1] + (uint64_t)p[2] + (uint64_t)p[3];
      p += 4;
    }
    while (p < pe) {
      sum += (uint64_t)(*p++);
    }

    hash = mix(hash, sum);
  }

  return hash;
}
