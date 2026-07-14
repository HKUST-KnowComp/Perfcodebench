#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = dictionary.size();
  const std::size_t m = codes.size();
  std::vector<uint64_t> sums(n, 0);
  uint64_t hash;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    for (std::size_t i = 0; i < m; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < n; ++j) {
      hash = mix(hash, sums[j]);
    }
  }
  return hash;
}
