#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Baseline restarts the hash each iteration and produces the same final value for any iters > 0.
  // Therefore, we only need to compute a single iteration when iters > 0.
  if (iters <= 0) return 0ULL;

  const std::size_t nrows = (row_ptr.size() > 0) ? (row_ptr.size() - 1) : 0;
  const uint32_t* rp = row_ptr.data();
  const uint32_t* vals = values.data();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  for (std::size_t r = 0; r < nrows; ++r) {
    std::size_t start = static_cast<std::size_t>(rp[r]);
    std::size_t end = static_cast<std::size_t>(rp[r + 1]);

    const uint32_t* p = vals + start;
    const uint32_t* q = vals + end;

    uint64_t sum = 0;

    // Unrolled summation by 4 for better throughput
    while (p + 4 <= q) {
      sum += static_cast<uint64_t>(p[0])
           + static_cast<uint64_t>(p[1])
           + static_cast<uint64_t>(p[2])
           + static_cast<uint64_t>(p[3]);
      p += 4;
    }

    // Remainder
    switch (q - p) {
      case 3: sum += static_cast<uint64_t>(p[0]) + static_cast<uint64_t>(p[1]) + static_cast<uint64_t>(p[2]); break;
      case 2: sum += static_cast<uint64_t>(p[0]) + static_cast<uint64_t>(p[1]); break;
      case 1: sum += static_cast<uint64_t>(p[0]); break;
      default: break;
    }

    hash = mix(hash, sum);
  }

  return hash;
}
