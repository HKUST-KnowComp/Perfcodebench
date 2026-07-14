#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  if (iters <= 0) return 0ULL;

  const size_t n = (keys.size() < values.size()) ? keys.size() : values.size();
  const size_t group_count = (groups > 0) ? static_cast<size_t>(groups) : 0ULL;
  std::vector<uint64_t> sums(group_count, 0ULL);

  uint64_t out = 0ULL;
  const uint32_t g_u32 = static_cast<uint32_t>(group_count);

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into dense buckets, guard against out-of-range keys
    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();
    uint64_t* agg = sums.data();

    for (size_t i = 0; i < n; ++i) {
      const uint32_t k = kptr[i];
      if (k < g_u32) {
        agg[k] += static_cast<uint64_t>(vptr[i]);
      }
    }

    // Compute FNV-1a checksum while zeroing for the next iteration
    uint64_t hash = 1469598103934665603ULL;
    uint64_t* p = agg;
    for (size_t g = 0; g < group_count; ++g) {
      uint64_t v = p[g];
      hash ^= v;
      hash *= 1099511628211ULL;
      p[g] = 0ULL;
    }
    out = hash;
  }

  return out;
}
