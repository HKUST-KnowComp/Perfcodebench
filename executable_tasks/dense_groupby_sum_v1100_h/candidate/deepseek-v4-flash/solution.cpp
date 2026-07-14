#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> sums(groups, 0);
  uint64_t out = 0;
  const auto keys_ptr = keys.data();
  const auto vals_ptr = values.data();
  const size_t n = keys.size();
  const int g = groups;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      const uint32_t k = keys_ptr[i];
      if (k < static_cast<uint32_t>(g)) {
        sums[k] += vals_ptr[i];
      }
    }
    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < g; ++i) {
      uint64_t v = sums[i];
      hash ^= v;
      hash *= 1099511628211ULL;
      sums[i] = 0;
    }
    out = hash;
  }
  return out;
}