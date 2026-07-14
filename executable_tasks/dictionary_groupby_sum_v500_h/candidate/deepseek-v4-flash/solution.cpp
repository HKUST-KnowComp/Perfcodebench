#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <string>
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
  const std::size_t dict_size = dictionary.size();
  const std::size_t data_size = codes.size();
  std::vector<uint64_t> sums(dict_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator
    std::fill(sums.begin(), sums.end(), 0ULL);
    // Aggregate values by dictionary code
    for (std::size_t i = 0; i < data_size; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    // Compute checksum in dictionary order using FNV-1 mix
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}
