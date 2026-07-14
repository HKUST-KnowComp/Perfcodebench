#include "interface.h"

#include <string>
#include <vector>

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
  const size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums to zero for this iteration
    std::fill(sums.begin(), sums.end(), 0ULL);

    // Aggregate directly using codes as indices
    const size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
      sums[static_cast<size_t>(codes[i])] += static_cast<uint64_t>(values[i]);
    }

    // Compute FNV-1a hash in dictionary order
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }

  return hash;
}
