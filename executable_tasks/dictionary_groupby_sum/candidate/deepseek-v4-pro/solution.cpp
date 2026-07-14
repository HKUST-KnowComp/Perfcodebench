#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();
  std::vector<uint64_t> sums(dict_size, 0);
  for (std::size_t i = 0; i < n; ++i) {
    sums[codes[i]] += values[i];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < dict_size; ++i) {
    hash ^= sums[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}