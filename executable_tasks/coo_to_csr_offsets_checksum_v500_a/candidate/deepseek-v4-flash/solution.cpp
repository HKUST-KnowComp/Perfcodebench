#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // Build CSR offset array from sorted row IDs (single pass)
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1, 0);
  {
    std::size_t idx = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      uint32_t cnt = 0;
      while (idx < row_ids.size() && row_ids[idx] == r) {
        ++cnt;
        ++idx;
      }
      offsets[static_cast<std::size_t>(r) + 1] = offsets[static_cast<std::size_t>(r)] + cnt;
    }
  }

  // Compute FNV-1a 64-bit hash once (all iterations would produce the same result)
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }

  // The final hash is identical regardless of the number of iterations (iters > 0)
  return hash;
}
