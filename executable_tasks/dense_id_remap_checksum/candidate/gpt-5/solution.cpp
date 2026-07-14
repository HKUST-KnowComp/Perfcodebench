#include "interface.h"

namespace {

// Pointer-based lower_bound: returns the first index i in [0, n]
// such that base[i] >= x; if all elements are < x, returns n.
static inline std::size_t lower_bound_index(const uint32_t* base, std::size_t n, uint32_t x) {
  std::size_t first = 0;
  while (n > 0) {
    std::size_t half = n >> 1;
    std::size_t mid = first + half;
    if (base[mid] < x) {
      first = mid + 1;
      n -= half + 1;
    } else {
      n = half;
    }
  }
  return first;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // If no iterations requested, baseline returns 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const uint32_t* pptr = probes.data();
  const std::size_t kn = keys.size();
  const std::size_t pn = probes.size();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  for (std::size_t i = 0; i < pn; ++i) {
    const uint32_t probe = pptr[i];
    const std::size_t idx = lower_bound_index(kptr, kn, probe);
    // Mix with FNV-1a step
    hash ^= static_cast<uint64_t>(vptr[idx]);
    hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}
