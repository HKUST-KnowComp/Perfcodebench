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
  
  // Pre-allocate aggregation buffer to avoid repeated allocations
  std::vector<uint64_t> sums(dict_size);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    std::fill(sums.begin(), sums.end(), 0);

    // Aggregate values using the codes as direct indices
    // This avoids string decoding and map overhead
    for (size_t i = 0; i < data_size; ++i) {
      sums[codes[i]] += values[i];
    }

    // Compute checksum based on dictionary order
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      current_hash = mix(current_hash, sums[i]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}