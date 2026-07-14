#include "interface.h"

#include <cstdint>
#include <vector>

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
  // Match baseline: if iters <= 0, return 0 without computing rows.
  if (iters <= 0) return 0;

  const std::size_t num_rows =
      (row_ptr.size() > 0) ? row_ptr.size() - 1 : 0;

  // Precompute row sums once.
  std::vector<uint64_t> row_sums(num_rows);
  for (std::size_t r = 0; r < num_rows; ++r) {
    uint64_t sum = 0;
    const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
    const std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
    for (std::size_t i = start; i < end; ++i) {
      sum += static_cast<uint64_t>(values[i]);
    }
    row_sums[r] = sum;
  }

  const uint64_t init = 1469598103934665603ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = init;
    for (const auto& s : row_sums) {
      hash = mix(hash, s);
    }
  }

  return hash;
}
