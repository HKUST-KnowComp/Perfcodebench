#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums to zero
    std::fill(sums.begin(), sums.end(), 0);

    // Accumulate values by code (dictionary index)
    for (std::size_t i = 0; i < codes.size(); ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute hash in dictionary order
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
    final_hash = hash;
  }

  return final_hash;
}
