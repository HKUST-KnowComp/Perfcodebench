#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);

  // Aggregate once: dictionary indices directly used for array updates.
  for (size_t i = 0; i < codes.size(); ++i) {
    sums[codes[i]] += static_cast<uint64_t>(values[i]);
  }

  uint64_t hash = 0;
  const uint64_t seed = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (size_t j = 0; j < dict_size; ++j) {
      hash ^= sums[j];
      hash *= mul;
    }
  }

  return hash;
}
