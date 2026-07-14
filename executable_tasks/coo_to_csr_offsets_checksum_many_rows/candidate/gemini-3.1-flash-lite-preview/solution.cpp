#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0);
  const size_t n = row_ids.size();

  // Since row_ids are sorted, we can compute offsets in O(N + rows)
  // offsets[i] is the count of elements with row_id < i
  size_t current_row = 0;
  for (size_t i = 0; i < n; ++i) {
    uint32_t r = row_ids[i];
    while (current_row <= r) {
      offsets[current_row++] = static_cast<uint32_t>(i);
    }
  }
  while (current_row <= rows) {
    offsets[current_row++] = static_cast<uint32_t>(n);
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}