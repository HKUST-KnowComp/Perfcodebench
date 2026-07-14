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
  const std::size_t dict_size = dictionary.size();
  const std::size_t entry_count = codes.size();
  std::vector<uint64_t> sums(dict_size, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);

    for (std::size_t i = 0; i < entry_count; ++i) {
      const std::size_t code_idx = static_cast<std::size_t>(codes[i]);
      sums[code_idx] += static_cast<uint64_t>(values[i]);
    }

    final_hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      final_hash = mix(final_hash, sums[i]);
    }
  }
  return final_hash;
}