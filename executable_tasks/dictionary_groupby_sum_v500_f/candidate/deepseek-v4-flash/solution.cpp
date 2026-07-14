#include "interface.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);

    const std::size_t n = codes.size();
    for (std::size_t i = 0; i < n; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      uint64_t val = sums[i];
      hash ^= val;
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}