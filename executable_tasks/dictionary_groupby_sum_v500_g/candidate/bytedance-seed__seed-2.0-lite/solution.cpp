#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <string>

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
  const size_t num_keys = dictionary.size();
  std::vector<uint64_t> sums(num_keys, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);

    for (size_t i = 0; i < codes.size(); ++i) {
      const size_t key_idx = static_cast<size_t>(codes[i]);
      sums[key_idx] += static_cast<uint64_t>(values[i]);
    }

    hash = 1469598103934665603ULL;
    for (uint64_t key_sum : sums) {
      hash = mix(hash, key_sum);
    }
  }
  return hash;
}