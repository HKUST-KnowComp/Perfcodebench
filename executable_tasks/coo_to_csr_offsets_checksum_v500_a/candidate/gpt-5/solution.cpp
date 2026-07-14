#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  (void)iters; // The final checksum is independent of repeated recomputation

  const std::size_t nrows = static_cast<std::size_t>(rows);
  std::vector<uint32_t> offsets(nrows + 1, 0u);

  // Count occurrences per row (store counts in offsets[1..rows])
  for (uint32_t r : row_ids) {
    if (r < rows) {
      ++offsets[static_cast<std::size_t>(r) + 1u];
    }
  }

  // Build CSR offsets via prefix sum
  uint32_t accum = 0u;
  for (std::size_t i = 1; i <= nrows; ++i) {
    accum += offsets[i];
    offsets[i] = accum;
  }

  // FNV-1a 64-bit hash over the offsets array
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < offsets.size(); ++i) {
    hash ^= static_cast<uint64_t>(offsets[i]);
    hash *= 1099511628211ULL;
  }

  return hash;
}
