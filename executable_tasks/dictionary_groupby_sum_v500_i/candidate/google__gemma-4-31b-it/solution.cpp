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
  const size_t data_size = codes.size();
  
  // Use a vector as a direct-address table for the codes
  // Since codes are uint16_t, the max size is 65536, but we only need dict_size
  std::vector<uint64_t> sums(dict_size);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    std::fill(sums.begin(), sums.end(), 0);

    // Aggregate values by code
    for (size_t i = 0; i < data_size; ++i) {
      sums[codes[i]] += values[i];
    }

    // Calculate checksum based on dictionary order
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      current_hash = mix(current_hash, sums[i]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}