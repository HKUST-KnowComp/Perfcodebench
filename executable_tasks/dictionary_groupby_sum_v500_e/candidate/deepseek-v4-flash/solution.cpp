#include "interface.h"

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
  std::size_t m = dictionary.size();
  std::size_t n = codes.size();
  std::vector<uint64_t> sums(m);
  for (int iter = 0; iter < iters; ++iter) {
    sums.assign(m, 0);
    for (std::size_t i = 0; i < n; ++i) {
      sums[codes[i]] += values[i];
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < m; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}