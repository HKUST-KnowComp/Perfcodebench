#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a prime
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) return 0ULL;  // Preserve baseline behavior for iters==0

  const std::size_t nrows = row_ptr.size() > 0 ? (row_ptr.size() - 1) : 0;
  const uint32_t* rp = row_ptr.data();
  const uint32_t* vals = values.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  for (std::size_t r = 0; r < nrows; ++r) {
    std::size_t start = static_cast<std::size_t>(rp[r]);
    std::size_t end   = static_cast<std::size_t>(rp[r + 1]);

    const uint32_t* p = vals + start;
    std::size_t len = end - start;

    uint64_t sum = 0;

    // Unroll by 8 for throughput
    while (len >= 8) {
      sum += p[0];
      sum += p[1];
      sum += p[2];
      sum += p[3];
      sum += p[4];
      sum += p[5];
      sum += p[6];
      sum += p[7];
      p += 8;
      len -= 8;
    }
    // Remainder
    for (std::size_t i = 0; i < len; ++i) {
      sum += p[i];
    }

    hash = mix(hash, sum);
  }

  return hash;
}
