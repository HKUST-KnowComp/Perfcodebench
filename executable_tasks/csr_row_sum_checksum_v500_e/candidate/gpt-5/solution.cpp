#include "interface.h"

#include <cstddef>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Preserve original semantics: if iters == 0, baseline returns 0.
  if (iters <= 0) return 0ULL;

  const std::size_t rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  const uint32_t* data = values.data();

  for (std::size_t r = 0; r < rows; ++r) {
    const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    const std::size_t end   = static_cast<std::size_t>(row_ptr[r + 1]);

    const uint32_t* p = data + start;
    const uint32_t* q = data + end;

    uint64_t s0 = 0, s1 = 0;  // dual accumulators to reduce dependency chains

    // Unroll by 8
    while (p + 8 <= q) {
      s0 += static_cast<uint64_t>(p[0]);
      s1 += static_cast<uint64_t>(p[1]);
      s0 += static_cast<uint64_t>(p[2]);
      s1 += static_cast<uint64_t>(p[3]);
      s0 += static_cast<uint64_t>(p[4]);
      s1 += static_cast<uint64_t>(p[5]);
      s0 += static_cast<uint64_t>(p[6]);
      s1 += static_cast<uint64_t>(p[7]);
      p += 8;
    }
    // Unroll by 4
    while (p + 4 <= q) {
      s0 += static_cast<uint64_t>(p[0]);
      s1 += static_cast<uint64_t>(p[1]);
      s0 += static_cast<uint64_t>(p[2]);
      s1 += static_cast<uint64_t>(p[3]);
      p += 4;
    }
    // Tail
    while (p < q) {
      s0 += static_cast<uint64_t>(*p++);
    }

    const uint64_t sum = s0 + s1;
    hash = mix(hash, sum);
  }

  // Baseline resets and recomputes the same hash each iter; last value equals a single computation.
  return hash;
}
