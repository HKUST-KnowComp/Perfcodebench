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
  uint64_t hash = 0;
  const size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  const size_t n = codes.size();
  const uint16_t* p_codes = codes.data();
  const uint32_t* p_values = values.data();
  uint64_t* p_sums = sums.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    for (size_t i = 0; i < n; ++i) {
      p_sums[p_codes[i]] += p_values[i];
    }
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, p_sums[i]);
    }
  }
  return hash;
}
