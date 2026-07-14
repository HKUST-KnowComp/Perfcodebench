#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const std::size_t n_offsets = static_cast<std::size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(n_offsets);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Zero out the offsets array (reuse allocation)
    std::memset(offsets.data(), 0, n_offsets * sizeof(uint32_t));

    // Count occurrences of each row id directly into offsets[row+1]
    // This way offsets[r+1] will hold count of row r
    for (uint32_t row : row_ids) {
      ++offsets[static_cast<std::size_t>(row) + 1u];
    }

    // Prefix sum to convert counts to offsets
    for (std::size_t r = 1; r < n_offsets; ++r) {
      offsets[r] += offsets[r - 1];
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* ptr = offsets.data();
    std::size_t i = 0;
    const std::size_t len = n_offsets;

    // Unroll by 4
    for (; i + 4 <= len; i += 4) {
      hash ^= ptr[i];
      hash *= 1099511628211ULL;
      hash ^= ptr[i + 1];
      hash *= 1099511628211ULL;
      hash ^= ptr[i + 2];
      hash *= 1099511628211ULL;
      hash ^= ptr[i + 3];
      hash *= 1099511628211ULL;
    }
    // Handle remainder
    for (; i < len; ++i) {
      hash ^= ptr[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
