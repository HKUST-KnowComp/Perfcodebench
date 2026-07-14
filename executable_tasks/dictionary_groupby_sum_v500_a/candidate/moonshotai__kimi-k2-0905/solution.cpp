#include "interface.h"

#include <cstdint>
#include <vector>

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
  const size_t n = codes.size();
  const size_t dict_size = dictionary.size();

  // Pre-size a vector to hold per-code sums; index == code.
  std::vector<uint64_t> sums(dict_size, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Clear sums in O(dict_size) instead of O(n) via memset-like fill.
    std::fill(sums.begin(), sums.end(), 0);

    // Single linear pass: accumulate values by code.
    for (size_t i = 0; i < n; ++i) {
      sums[static_cast<size_t>(codes[i])] += static_cast<uint64_t>(values[i]);
    }

    // Mix sums in dictionary order.
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}