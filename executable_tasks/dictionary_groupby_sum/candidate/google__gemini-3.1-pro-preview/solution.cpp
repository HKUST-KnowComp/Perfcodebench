#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

/**
 * The mix function provided in the baseline, used for FNV-like hashing.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

/**
 * Optimizes the dictionary-coded group-by aggregation.
 * 
 * The baseline implementation was slow because it decoded codes back into strings
 * and used a std::unordered_map<std::string, uint64_t> for aggregation.
 * 
 * Optimization Strategy:
 * 1. Use the 'codes' (uint16_t) directly as indices into a fixed-size accumulation array.
 * 2. Avoid string operations entirely during the aggregation phase.
 * 3. Pre-allocate the accumulation buffer to avoid repeated allocations.
 * 4. Use a tight loop for the aggregation and hashing.
 */
uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  
  const size_t dict_size = dictionary.size();
  const size_t num_elements = codes.size();
  
  // Pre-allocate a buffer for sums. Since codes are uint16_t, 
  // the dictionary size is at most 65536.
  std::vector<uint64_t> sums(dict_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    std::fill(sums.begin(), sums.end(), 0ULL);

    // Aggregate values by code index
    // This is the hot loop. Accessing sums[codes[i]] is efficient as long as
    // the sums vector fits in cache (max ~512KB for 64k entries).
    const uint16_t* __restrict p_codes = codes.data();
    const uint32_t* __restrict p_values = values.data();
    for (size_t i = 0; i < num_elements; ++i) {
      sums[p_codes[i]] += p_values[i];
    }

    // Calculate checksum in dictionary order
    // FNV-1a offset basis
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }

  return hash;
}
