#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix_hash(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) return 0ULL;

  const uint32_t* rid = row_ids.data();
  const size_t nnz = row_ids.size();

  std::vector<uint32_t> offsets;
  offsets.resize(static_cast<size_t>(rows) + 1u);

  offsets[0] = 0u;
  size_t pos = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    while (pos < nnz && rid[pos] == r) {
      ++pos;
    }
    offsets[static_cast<size_t>(r) + 1u] = static_cast<uint32_t>(pos);
  }

  uint64_t hash = 1469598103934665603ULL;
  const size_t off_sz = offsets.size();
  for (size_t i = 0; i < off_sz; ++i) {
    hash = mix_hash(hash, offsets[i]);
  }
  return hash;
}
