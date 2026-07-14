#include "interface.h"

#include <cstdint>
#include <cstring>
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
  const size_t n = codes.size();
  const size_t dict_size = dictionary.size();

  // Per-code accumulators, reused every iteration.
  std::vector<uint64_t> sum(dict_size, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Zero the accumulators quickly.
    std::memset(sum.data(), 0, dict_size * sizeof(uint64_t));

    // Single-pass aggregation by code.
    for (size_t i = 0; i < n; ++i) {
      const uint16_t c = codes[i];
      sum[c] += static_cast<uint64_t>(values[i]);
    }

    // Compute checksum in dictionary order.
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sum[i]);
    }
  }
  return hash;
}