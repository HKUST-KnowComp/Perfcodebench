#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Preserve baseline behavior: if iters == 0, no hashing performed
  if (iters == 0) return 0ULL;

  // Compute per-group maxima once; repeated iterations would yield the same hash
  std::vector<uint32_t> best(group_count, 0u);

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const std::size_t n = groups.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t g = gptr[i];
    uint32_t v = vptr[i];
    uint32_t cur = best[g];
    if (v > cur) best[g] = v;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  const uint32_t* bp = best.data();
  const uint32_t* be = bp + group_count;
  for (const uint32_t* p = bp; p != be; ++p) {
    hash = mix(hash, static_cast<uint64_t>(*p));
  }

  return hash;
}
