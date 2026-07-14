#include "interface.h"

#include <vector>
#include <cstring>

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
  std::vector<uint64_t> sums(dictionary.size());
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(sums.data(), 0, sums.size() * sizeof(uint64_t));
    for (std::size_t i = 0; i < codes.size(); ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < dictionary.size(); ++j) {
      hash = mix(hash, sums[j]);
    }
  }
  return hash;
}