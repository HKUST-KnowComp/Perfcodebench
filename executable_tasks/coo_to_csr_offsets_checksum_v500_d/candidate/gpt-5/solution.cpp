#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Match baseline behavior for iters == 0
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* data = row_ids.data();
  const std::size_t nnz = row_ids.size();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  // offsets[0] = 0
  hash = mix(hash, 0ULL);

  // Build CSR offsets cumulative counts from sorted COO row_ids, hashing on the fly.
  std::size_t idx = 0;
  uint32_t cum = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    std::size_t start = idx;
    while (idx < nnz && data[idx] == r) {
      ++idx;
    }
    uint32_t count = static_cast<uint32_t>(idx - start);
    cum += count;
    hash = mix(hash, static_cast<uint64_t>(cum));
  }

  return hash;
}
