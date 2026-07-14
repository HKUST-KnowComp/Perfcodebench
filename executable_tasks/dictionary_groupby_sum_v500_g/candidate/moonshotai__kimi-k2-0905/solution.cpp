#include "interface.h"

#include <cstdint>
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
  const std::size_t n_codes = codes.size();
  const std::size_t dict_size = dictionary.size();

  // one-time reusable buffer for per-code sums
  std::vector<uint64_t> sums(dict_size, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // zero out sums vector in O(dict_size) – much cheaper than rebuilding map
    for (std::size_t k = 0; k < dict_size; ++k) sums[k] = 0;

    // single pass accumulation
    for (std::size_t i = 0; i < n_codes; ++i) {
      sums[static_cast<std::size_t>(codes[i])] += static_cast<uint64_t>(values[i]);
    }

    // compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (std::size_t k = 0; k < dict_size; ++k) {
      hash = mix(hash, sums[k]);
    }
  }
  return hash;
}