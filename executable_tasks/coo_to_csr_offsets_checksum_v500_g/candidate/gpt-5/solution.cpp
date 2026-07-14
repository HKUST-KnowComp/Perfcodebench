#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t mix64(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL; // FNV-1a 64-bit prime
  return h;
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  const uint32_t* p = row_ids.data();
  const uint32_t* end = p + row_ids.size();

  // FNV-1a 64-bit offset basis
  uint64_t hash = 1469598103934665603ULL;

  // offsets[0] = 0
  hash = mix64(hash, static_cast<uint64_t>(0u));

  uint32_t idx = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    while (p != end && *p == r) {
      ++p;
      ++idx;
    }
    // offsets[r + 1] = idx
    hash = mix64(hash, static_cast<uint64_t>(idx));
  }

  return hash;
}
