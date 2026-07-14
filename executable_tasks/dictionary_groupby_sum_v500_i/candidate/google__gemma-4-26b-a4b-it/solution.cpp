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
  
  if (codes.empty()) {
    return 0;
  }

  // Pre-allocate accumulation buffer to avoid reallocations.
  // The size is based on the dictionary size to allow direct indexing.
  std::vector<uint64_t> sums(dictionary.size(), 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for the current iteration
    std::fill(sums.begin(), sums.end(), 0);

    // Aggregate values using the codes as direct indices.
    // This avoids string decoding and string-based hash map lookups.
    const size_t n = codes.size();
    const uint16_t* c_ptr = codes.data();
    const uint32_t* v_ptr = values.data();
    
    for (size_t i = 0; i < n; ++i) {
      sums[static_cast<size_t>(c_ptr[i])] += static_cast<uint64_t>(v_ptr[i]);
    }

    // Compute the checksum in dictionary order.
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dictionary.size(); ++i) {
      hash = mix(hash, sums[i]);
    }
  }

  return hash;
}