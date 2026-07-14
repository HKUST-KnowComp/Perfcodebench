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
  uint64_t final_hash = 0;
  std::vector<uint64_t> sums(dictionary.size(), 0);
  const size_t num_codes = codes.size();
  const size_t dict_size = dictionary.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);

    for (size_t i = 0; i < num_codes; ++i) {
      const size_t code = static_cast<size_t>(codes[i]);
      sums[code] += static_cast<uint64_t>(values[i]);
    }

    final_hash = 1469598103934665603ULL;
    for (size_t j = 0; j < dict_size; ++j) {
      final_hash = mix(final_hash, sums[j]);
    }
  }

  return final_hash;
}