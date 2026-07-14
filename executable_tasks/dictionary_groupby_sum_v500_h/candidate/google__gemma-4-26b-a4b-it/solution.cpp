#include "interface.h"

#include <string>
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
  if (dictionary.empty()) return 0;

  const size_t dict_size = dictionary.size();
  const size_t num_elements = codes.size();
  
  // Pre-allocate sums vector to avoid re-allocation in the loop.
  // We use the index directly since 'codes' are indices into 'dictionary'.
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for each iteration
    for (size_t i = 0; i < dict_size; ++i) {
      sums[i] = 0;
    }

    // Direct aggregation using codes as indices
    for (size_t i = 0; i < num_elements; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }

    // Calculate checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }

  return hash;
}