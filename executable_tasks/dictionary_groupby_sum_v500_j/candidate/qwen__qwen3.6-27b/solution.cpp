#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;
  size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);

  const uint16_t* c = codes.data();
  const uint32_t* v = values.data();
  size_t n = codes.size();

  for (size_t i = 0; i < n; ++i) {
    sums[c[i]] += v[i];
  }

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < dict_size; ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}