#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV prime
  return hash;
}

constexpr uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t nrows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  const uint32_t* vals = values.data();

  uint64_t hash = kFNVOffsetBasis;

  for (std::size_t r = 0; r < nrows; ++r) {
    const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    const std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
    const std::size_t len = (end >= start) ? (end - start) : 0;

    const uint32_t* p = vals + start;

    uint64_t sum = 0ULL;
    std::size_t i = 0;
    std::size_t unrolled = len & ~static_cast<std::size_t>(3);
    for (; i < unrolled; i += 4) {
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
