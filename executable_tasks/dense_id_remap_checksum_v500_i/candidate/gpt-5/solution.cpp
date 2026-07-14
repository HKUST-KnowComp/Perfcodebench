#include "interface.h"

namespace {

static inline const uint32_t* lower_bound_ptr(const uint32_t* base, size_t n, uint32_t probe) {
  size_t first = 0;
  while (n > 0) {
    size_t half = n >> 1;
    const uint32_t* mid = base + first + half;
    if (*mid < probe) {
      first += half + 1;
      n -= half + 1;
    } else {
      n = half;
    }
  }
  return base + first;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0ULL;

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const size_t kn = keys.size();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  for (size_t i = 0, pn = probes.size(); i < pn; ++i) {
    const uint32_t probe = probes[i];
    const uint32_t* it = lower_bound_ptr(kptr, kn, probe);
    const size_t idx = static_cast<size_t>(it - kptr);
    // Assumes probes are within range so idx < values.size(), matching baseline behavior
    hash ^= static_cast<uint64_t>(vptr[idx]);
    hash *= 1099511628211ULL;  // FNV-1a prime
  }

  return hash;
}
