#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// ---- fast checksum --------------------------------------------------------
inline uint64_t checksum_vec(const uint64_t* __restrict sums,
                             int groups) noexcept {
  uint64_t h = 1469598103934665603ULL;
  int g = 0;

  // 4-way unroll
  for (; g + 3 < groups; g += 4) {
    h ^= sums[g];
    h *= 1099511628211ULL;
    h ^= sums[g + 1];
    h *= 1099511628211ULL;
    h ^= sums[g + 2];
    h *= 1099511628211ULL;
    h ^= sums[g + 3];
    h *= 1099511628211ULL;
  }
  for (; g < groups; ++g) {
    h ^= sums[g];
    h *= 1099511628211ULL;
  }
  return h;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                          const std::vector<uint32_t>& values,
                          int groups,
                          int iters) {
  const std::size_t n = keys.size();
  std::vector<uint64_t> sums(groups, 0ULL);

  // Pre-compute pointers to avoid bound checks inside hot loop
  const uint32_t* __restrict k = keys.data();
  const uint32_t* __restrict v = values.data();

  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // clear vector contents quickly
    std::fill(sums.begin(), sums.end(), 0ULL);

    // tight update loop with 4-way unroll
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      sums[k[i]]     += v[i];
      sums[k[i + 1]] += v[i + 1];
      sums[k[i + 2]] += v[i + 2];
      sums[k[i + 3]] += v[i + 3];
    }
    for (; i < n; ++i) {
      sums[k[i]] += v[i];
    }

    out = checksum_vec(sums.data(), groups);
  }
  return out;
}