#include "interface.h"
#include <vector>
#include <cstdint>

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
  const size_t dict_size = dictionary.size();
  const size_t num_codes = codes.size();
  std::vector<uint64_t> sums(dict_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    std::fill(sums.begin(), sums.end(), 0);

    // Aggregate directly using codes as indices
    for (size_t i = 0; i < num_codes; ++i) {
      sums[codes[i]] += values[i];
    }

    // Compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}