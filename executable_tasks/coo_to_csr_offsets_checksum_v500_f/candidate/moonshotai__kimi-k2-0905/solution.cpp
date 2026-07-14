#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows,
                          int iters) {
  // Allocate reusable buffers once, outside the timing loop
  std::vector<uint32_t> offsets(rows + 1);
  std::vector<uint32_t> counts(rows, 0u);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // 1. Zero-fill counts array (faster than re-creating vector)
    std::memset(counts.data(), 0, rows * sizeof(uint32_t));

    // 2. Single pass: accumulate row frequencies
    for (uint32_t r : row_ids) {
      ++counts[r];
    }

    // 3. Prefix-sum (exclusive scan) to build offsets
    uint64_t acc = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      acc += counts[r];
      offsets[r + 1] = static_cast<uint32_t>(acc);
    }

    // 4. Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}