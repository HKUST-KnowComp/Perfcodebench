#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t D = dictionary.size();
  const std::size_t N = codes.size();
  std::vector<uint64_t> sums(D);
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* values_ptr = values.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0ULL);
    for (std::size_t i = 0; i < N; ++i) {
      sums[codes_ptr[i]] += values_ptr[i];
    }
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < D; ++j) {
      uint64_t s = sums[j];
      hash ^= s;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}