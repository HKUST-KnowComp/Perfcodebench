#include "interface.h"

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
  const size_t dict_size = dictionary.size();
  const size_t num_entries = codes.size();
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);

    for (size_t i = 0; i < num_entries; ++i) {
      const uint16_t code = codes[i];
      sums[code] += values[i];
    }

    uint64_t hash = 1469598103934665603ULL;
    for (size_t k = 0; k < dict_size; ++k) {
      hash = mix(hash, sums[k]);
    }
    final_hash = hash;
  }

  return final_hash;
}