#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a prime
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  // Compute CSR row offsets hash directly in a streaming fashion without materializing the offsets array.
  // Assumes row_ids are sorted non-decreasing.
  const uint32_t* ids = row_ids.empty() ? nullptr : row_ids.data();
  const size_t n = row_ids.size();
  size_t i = 0;  // number of entries consumed up to current row

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  // offsets[0] = 0
  hash = mix(hash, 0u);

  for (uint32_t r = 0; r < rows; ++r) {
    // Advance over all entries belonging to row r
    while (i < n && ids[i] == r) {
      ++i;
    }
    // offsets[r+1] = i (cumulative count up to and including row r)
    hash = mix(hash, static_cast<uint32_t>(i));
  }

  return hash;
}
