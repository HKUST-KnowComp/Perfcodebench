#include "interface.h"
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  uint64_t hash = 1469598103934665603ULL;
  uint32_t current_offset = 0;

  // Hash offsets[0] which is always 0
  hash ^= current_offset;
  hash *= 1099511628211ULL;

  size_t idx = 0;
  const size_t n = row_ids.size();

  for (uint32_t r = 0; r < rows; ++r) {
    uint32_t count = 0;
    while (idx < n && row_ids[idx] == r) {
      ++count;
      ++idx;
    }
    current_offset += count;
    hash ^= current_offset;
    hash *= 1099511628211ULL;
  }

  (void)iters;
  return hash;
}