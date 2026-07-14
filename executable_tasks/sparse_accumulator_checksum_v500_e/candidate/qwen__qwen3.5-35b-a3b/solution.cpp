#include "interface.h"

#include <vector>
#include <algorithm>
#include <unordered_map>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t multiply_by_C_pow(uint64_t h, uint32_t gap) {
    if (gap == 0) return h;
    uint64_t c = 1099511628211ULL;
    uint64_t result = 1;
    while (gap > 0) {
        if (gap & 1) result = result * c;
        c = c * c;
        gap >>= 1;
    }
    return h * result;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  std::unordered_map<uint32_t, uint64_t> accum;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    accum.clear();
    for (std::size_t i = 0; i < indices.size(); ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    std::vector<uint32_t> keys;
    keys.reserve(accum.size());
    for (const auto& [key, value] : accum) {
      keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    uint32_t current = 0;
    for (uint32_t key : keys) {
      uint32_t gap = key - current;
      hash = multiply_by_C_pow(hash, gap);
      hash = mix(hash, accum[key]);
      current = key + 1;
    }
    uint32_t remaining = domain - current;
    hash = multiply_by_C_pow(hash, remaining);
  }
  return hash;
}