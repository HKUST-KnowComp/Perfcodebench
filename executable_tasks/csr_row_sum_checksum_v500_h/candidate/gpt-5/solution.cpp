#include "interface.h"

#include <vector>

namespace {

constexpr uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= kFNVPrime;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Preserve baseline behavior: if iters == 0, return 0.
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = kFNVOffsetBasis;

  const uint32_t* vals = values.data();
  const uint32_t* rp = row_ptr.data();
  const std::size_t rows = row_ptr.size() > 0 ? (row_ptr.size() - 1) : 0;

  for (std::size_t r = 0; r < rows; ++r) {
    std::size_t start = static_cast<std::size_t>(rp[r]);
    std::size_t end = static_cast<std::size_t>(rp[r + 1]);

    const uint32_t* p = vals + start;
    std::size_t len = end - start;

    uint64_t sum = 0;

    // Unrolled accumulation for better throughput
    std::size_t i = 0;
    for (; i + 4 <= len; i += 4) {
      sum += static_cast<uint64_t>(p[i])
           + static_cast<uint64_t>(p[i + 1])
           + static_cast<uint64_t>(p[i + 2])
           + static_cast<uint64_t>(p[i + 3]);
    }
    for (; i < len; ++i) {
      sum += static_cast<uint64_t>(p[i]);
    }

    hash = mix(hash, sum);
  }

  return hash;
}
