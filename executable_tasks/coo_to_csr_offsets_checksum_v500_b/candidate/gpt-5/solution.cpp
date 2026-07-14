#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  // Compute CSR offsets from sorted COO row_ids via a single linear scan
  // and hash them on the fly using FNV-1a 64-bit (same as baseline mix).
  const uint32_t* ptr = row_ids.data();
  const uint32_t* end = ptr + row_ids.size();

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  // offsets[0] = 0
  hash = mix(hash, 0u);

  std::size_t idx = 0; // cumulative count up to current row
  for (uint32_t r = 0; r < rows; ++r) {
    // Advance through all entries that belong to row r
    while (ptr < end && *ptr == r) {
      ++ptr;
      ++idx;
    }
    // offsets[r + 1] = idx (cumulative NNZ up to row r)
    hash = mix(hash, static_cast<uint32_t>(idx));
  }

  return hash;
}
