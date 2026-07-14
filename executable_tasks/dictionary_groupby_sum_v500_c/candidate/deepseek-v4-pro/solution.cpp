#include "interface.h"

#include <cstddef>
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
  if (iters <= 0) return 0;

  const std::size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    sums[codes[i]] += values[i];
  }

  uint64_t hash = 0;  // will be overwritten in loop
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}