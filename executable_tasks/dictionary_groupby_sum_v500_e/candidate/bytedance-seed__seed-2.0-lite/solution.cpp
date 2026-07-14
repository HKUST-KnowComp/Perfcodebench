#include "interface.h"
#include <vector>
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
  uint64_t hash = 0;
  std::vector<uint64_t> sums(dictionary.size(), 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    for (std::size_t i = 0; i < codes.size(); ++i) {
      const std::size_t dict_idx = static_cast<std::size_t>(codes[i]);
      sums[dict_idx] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}