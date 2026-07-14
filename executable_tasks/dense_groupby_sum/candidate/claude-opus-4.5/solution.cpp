#include "interface.h"

#include <vector>
#include <cstring>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint32_t* __restrict v = values.data();
  
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));
  uint64_t out = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(agg.data(), 0, static_cast<std::size_t>(groups) * sizeof(uint64_t));
    
    std::size_t i = 0;
    const std::size_t unroll_end = n & ~static_cast<std::size_t>(3);
    
    for (; i < unroll_end; i += 4) {
      agg[k[i]]     += v[i];
      agg[k[i + 1]] += v[i + 1];
      agg[k[i + 2]] += v[i + 2];
      agg[k[i + 3]] += v[i + 3];
    }
    
    for (; i < n; ++i) {
      agg[k[i]] += v[i];
    }
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t* __restrict a = agg.data();
    
    for (int g = 0; g < groups; ++g) {
      hash ^= a[g];
      hash *= 1099511628211ULL;
    }
    
    out = hash;
  }
  
  return out;
}