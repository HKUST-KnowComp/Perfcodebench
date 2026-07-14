#include "interface.h"

#include <cstdint>
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
  (void)iters;
  std::vector<uint64_t> sums(dictionary.size(), 0);
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    sums[codes[i]] += values[i];
  }
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t d = dictionary.size();
  for (std::size_t i = 0; i < d; ++i) {
    hash = mix(hash, sums[i]);
  }
  return hash;
}