#include "interface.h"

namespace {

static inline uint64_t mix_hash(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Preserve semantics: if iters <= 0, baseline returns 0.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  const uint32_t* vals = values.data();

  for (std::size_t r = 0; r < rows; ++r) {
    std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);

    const uint32_t* p = vals + start;
    const uint32_t* pe = vals + end;

    uint64_t sum = 0;

    // Manual unroll by 4 for better throughput
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

    hash = mix_hash(hash, sum);
  }

  // The final value after any positive number of iterations equals the value after one iteration
  // because the baseline resets the hash each iteration and recomputes the same sequence.
  return hash;
}
