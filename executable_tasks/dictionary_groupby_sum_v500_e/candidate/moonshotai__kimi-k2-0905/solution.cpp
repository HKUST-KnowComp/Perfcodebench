#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));
    const std::size_t n = codes.size();
    for (std::size_t i = 0; i < n; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (std::size_t k = 0; k < dict_size; ++k) {
      hash = mix(hash, sums[k]);
    }
  }
  return hash;
}
