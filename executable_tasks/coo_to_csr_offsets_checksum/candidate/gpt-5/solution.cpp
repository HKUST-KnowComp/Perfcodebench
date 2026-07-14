#include "interface.h"

namespace {

static inline uint64_t mix64(uint64_t h, uint64_t v) {
  // FNV-1a step
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters == 0) return 0ULL;

  const std::size_t nrows = static_cast<std::size_t>(rows);
  std::vector<uint32_t> offsets(nrows + 1u, 0u);

  // Count entries per row (ignore out-of-range rows to match baseline behavior)
  for (uint32_t r : row_ids) {
    if (r < rows) {
      ++offsets[static_cast<std::size_t>(r) + 1u];
    }
  }

  // Exclusive prefix sum to build CSR offsets
  for (std::size_t i = 1; i <= nrows; ++i) {
    offsets[i] += offsets[i - 1];
  }

  // Compute FNV-1a hash over the offsets array
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  for (std::size_t i = 0, sz = offsets.size(); i < sz; ++i) {
    hash = mix64(hash, static_cast<uint64_t>(offsets[i]));
  }

  return hash;
}
