#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (dictionary.empty() || iters <= 0) {
    return 0;
  }
  // Preallocate sum vector once to match dictionary size for direct indexing
  std::vector<uint64_t> sums(dictionary.size(), 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    
    // Accumulate values via direct code indices, no string operations needed
    const size_t entry_count = codes.size();
    for (size_t i = 0; i < entry_count; ++i) {
      sums[static_cast<size_t>(codes[i])] += static_cast<uint64_t>(values[i]);
    }

    // Compute checksum in strict dictionary order, matching original logic exactly
    uint64_t hash = 1469598103934665603ULL;
    for (size_t j = 0; j < dictionary.size(); ++j) {
      hash = mix(hash, sums[j]);
    }
    final_hash = hash;
  }
  return final_hash;
}