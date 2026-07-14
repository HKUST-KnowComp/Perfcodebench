#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Handle groups potentially being negative by clamping to zero-sized storage.
  const std::size_t gsz = (groups > 0) ? static_cast<std::size_t>(groups) : 0;
  std::vector<uint64_t> sums(gsz, 0);

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const std::size_t n = keys.size();

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate values into dense buckets.
    // Assumes keys are within [0, groups) range per task specification.
    uint64_t* sptr = sums.data();

    // Simple aggregation loop
    for (std::size_t i = 0; i < n; ++i) {
      sptr[kptr[i]] += static_cast<uint64_t>(vptr[i]);
    }

    // Compute checksum over all groups and zero for next iteration in the same pass.
    uint64_t hash = FNV_OFFSET;
    for (std::size_t g = 0; g < gsz; ++g) {
      uint64_t v = sptr[g];
      hash ^= v;
      hash *= FNV_PRIME;
      sptr[g] = 0; // reset for next iteration
    }
    out = hash;
  }

  return out;
}
