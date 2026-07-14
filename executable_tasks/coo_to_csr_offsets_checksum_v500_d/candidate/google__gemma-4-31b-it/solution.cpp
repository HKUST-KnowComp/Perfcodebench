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
    // offsets[i] is the number of elements in rows 0 to i-1
    uint32_t current_row = 0;
    uint32_t count = 0;
    size_t n = row_ids.size();
    size_t i = 0;

    // Fill offsets array
    // We need to find how many elements belong to each row
    // Because row_ids is sorted, we can just count occurrences
    uint32_t running_offset = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r] = running_offset;
      while (i < n && row_ids[i] == r) {
        running_offset++;
        i++;
      }
    }
    offsets[rows] = running_offset;

    // Compute hash
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }

  return final_hash;
}