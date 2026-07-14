#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  // Build CSR row offsets via in-place histogram and prefix sum
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0u);

  // Histogram: count entries per row in offsets[1..rows]
  for (uint32_t r : row_ids) {
    // Assuming valid input: 0 <= r < rows
    offsets[static_cast<std::size_t>(r) + 1u] += 1u;
  }

  // Prefix sum to get CSR offsets
  uint32_t running = 0u;
  for (std::size_t i = 1; i < offsets.size(); ++i) {
    running += offsets[i];
    offsets[i] = running;
  }

  // Compute checksum (FNV-1-like as in baseline)
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash = mix(hash, v);
  }
  return hash;
}
