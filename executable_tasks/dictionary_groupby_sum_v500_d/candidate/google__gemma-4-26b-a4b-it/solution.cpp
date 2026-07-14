#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  
  const size_t dict_size = dictionary.size();
  const size_t num_elements = codes.size();
  
  // Pre-allocate the aggregation buffer to avoid reallocations
  std::vector<uint64_t> sums(dict_size);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    for (size_t i = 0; i < dict_size; ++i) {
      sums[i] = 0;
    }

    // Aggregate values using the codes as direct indices into the sums array
    // This bypasses string decoding and map lookups entirely
    for (size_t i = 0; i < num_elements; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute the checksum in dictionary order
    final_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      final_hash = mix(final_hash, sums[i]);
    }
  }

  return final_hash;
}
