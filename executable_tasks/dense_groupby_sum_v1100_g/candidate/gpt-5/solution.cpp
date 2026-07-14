#include "interface.h"

#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  uint64_t out = 0;

  const size_t n = keys.size();
  (void)n; // silence unused if not used in some builds

  // Prepare dense aggregator for groups; initialize to zero once.
  std::vector<uint64_t> agg;
  if (groups > 0) {
    agg.assign(static_cast<size_t>(groups), 0ULL);
  }

  const size_t gsz = (groups > 0) ? static_cast<size_t>(groups) : 0ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into groups, ignoring keys >= groups
    if (gsz != 0) {
      uint64_t* a = agg.data();
      const uint32_t* kptr = keys.data();
      const uint32_t* vptr = values.data();
      const size_t end = keys.size();
      for (size_t i = 0; i < end; ++i) {
        uint32_t k = kptr[i];
        if (static_cast<size_t>(k) < gsz) {
          a[k] += static_cast<uint64_t>(vptr[i]);
        }
      }
    }

    // Compute checksum (FNV-1a) in group order and clear for next iteration
    uint64_t hash = 1469598103934665603ULL;
    if (gsz != 0) {
      uint64_t* a = agg.data();
      for (size_t g = 0; g < gsz; ++g) {
        uint64_t v = a[g];
        hash ^= v;
        hash *= 1099511628211ULL;
        a[g] = 0ULL; // clear for next iteration
      }
    }

    out = hash;
  }

  return out;
}
