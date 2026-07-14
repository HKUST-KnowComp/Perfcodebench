#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

namespace {

/**
 * FNV-1a-like mixing function used in the baseline.
 */
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
  
  // Pre-allocate sums buffer to avoid re-allocation inside the loop.
  // Since codes are uint16_t, dict_size is at most 65536.
  std::vector<uint64_t> sums(dict_size);

  uint64_t final_hash = 0;
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* values_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for each iteration.
    std::fill(sums.begin(), sums.end(), 0ULL);

    // Aggregate values by code index.
    // Unrolling the loop slightly to help with instruction-level parallelism.
    size_t i = 0;
    for (; i + 3 < data_size; i += 4) {
      sums[codes_ptr[i]]   += values_ptr[i];
      sums[codes_ptr[i+1]] += values_ptr[i+1];
      sums[codes_ptr[i+2]] += values_ptr[i+2];
      sums[codes_ptr[i+3]] += values_ptr[i+3];
    }
    for (; i < data_size; ++i) {
      sums[codes_ptr[i]] += values_ptr[i];
    }

    // Calculate checksum based on the dictionary order.
    // The baseline uses the FNV-1a offset basis as the starting hash.
    uint64_t hash = 1469598103934665603ULL;
    for (size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, sums[j]);
    }
    final_hash = hash;
  }

  return final_hash;
}
