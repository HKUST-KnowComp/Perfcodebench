#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) return 0ULL;

  const uint32_t* ids = row_ids.data();
  const std::size_t n = row_ids.size();

  // Build and hash CSR offsets on the fly without materializing the array.
  // Offsets[0] = 0, then for r in [0, rows): offsets[r+1] = count of ids <= r.
  std::size_t j = 0;
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  // Hash offsets[0] = 0
  hash = mix(hash, 0u);

  for (uint32_t r = 0; r < rows; ++r) {
    while (j < n && ids[j] == r) {
      ++j;
    }
    // Offsets[r+1] equals j (cast to uint32_t to match baseline behavior)
    hash = mix(hash, static_cast<uint32_t>(j));
  }

  return hash;
}
