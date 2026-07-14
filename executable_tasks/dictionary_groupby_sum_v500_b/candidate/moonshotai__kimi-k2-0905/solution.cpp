#include "interface.h"

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
  const size_t n = codes.size();
  const size_t dict_size = dictionary.size();

  // Pre-compute 64-bit keys for every dictionary entry once.
  std::vector<uint64_t> keys(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    uint64_t h = 1469598103934665603ULL;
    const std::string& s = dictionary[i];
    for (char c : s) {
      h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
      h *= 1099511628211ULL;
    }
    keys[i] = h;
  }

  // Dense accumulator indexed by 16-bit code.
  std::vector<uint64_t> sums(dict_size, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums to zero.
    std::fill(sums.begin(), sums.end(), 0);

    // Vectorizable aggregation.
    for (size_t i = 0; i < n; ++i) {
      const uint16_t code = codes[i];
      sums[code] += static_cast<uint64_t>(values[i]);
    }

    // Compute checksum in dictionary order.
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}