#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) return 0;

  // Pre-allocate memory to avoid repeated allocations inside the loop
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Since row_ids are sorted, we can compute offsets in one pass
    // offsets[i] = number of elements in rows 0 to i-1
    uint32_t current_row = 0;
    uint32_t count = 0;
    size_t n = row_ids.size();
    size_t i = 0;

    // Fill offsets array
    // We need to find the first occurrence of each row index
    // Because row_ids is sorted, we can just count occurrences
    std::memset(offsets.data(), 0, (rows + 1) * sizeof(uint32_t));
    
    for (; i < n; ++i) {
      uint32_t row = row_ids[i];
      // Since it's sorted, we only need to increment the count for the current row
      // However, the most robust way to build CSR offsets from sorted COO is:
      // offsets[row + 1]++
      offsets[row + 1]++;
    }

    // Prefix sum to transform counts into offsets
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] += offsets[r];
    }

    // Compute hash
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      h = mix(h, v);
    }
    final_hash = h;
  }

  return final_hash;
}