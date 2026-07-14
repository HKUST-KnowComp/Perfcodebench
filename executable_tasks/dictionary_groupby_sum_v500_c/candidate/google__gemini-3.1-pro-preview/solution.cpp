#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

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
  const size_t num_elements = codes.size();
  
  // Pre-allocate sums buffer to avoid reallocations.
  // Since codes are uint16_t, the dictionary size is at most 65536.
  std::vector<uint64_t> sums(dict_size);

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    std::fill(sums.begin(), sums.end(), 0ULL);

    // Aggregate values by code index
    // This is the hot loop. Using raw pointers can help the compiler optimize.
    const uint16_t* __restrict p_codes = codes.data();
    const uint32_t* __restrict p_values = values.data();
    uint64_t* __restrict p_sums = sums.data();

    for (size_t i = 0; i < num_elements; ++i) {
      p_sums[p_codes[i]] += static_cast<uint64_t>(p_values[i]);
    }

    // Compute the checksum based on the dictionary order
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, p_sums[i]);
    }
    final_hash = hash;
  }

  return final_hash;
}
