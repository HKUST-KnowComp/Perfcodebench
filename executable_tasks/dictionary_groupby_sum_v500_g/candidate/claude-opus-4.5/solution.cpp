#include "interface.h"

#include <cstdint>
#include <cstring>
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
  const std::size_t n = codes.size();
  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict values_ptr = values.data();

  std::vector<uint64_t> sums(dict_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));

    // Aggregate values by dictionary code index
    std::size_t i = 0;
    // Unroll by 4 for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    uint64_t* __restrict sums_ptr = sums.data();
    for (; i < n4; i += 4) {
      sums_ptr[codes_ptr[i]]     += values_ptr[i];
      sums_ptr[codes_ptr[i + 1]] += values_ptr[i + 1];
      sums_ptr[codes_ptr[i + 2]] += values_ptr[i + 2];
      sums_ptr[codes_ptr[i + 3]] += values_ptr[i + 3];
    }
    for (; i < n; ++i) {
      sums_ptr[codes_ptr[i]] += values_ptr[i];
    }

    // Compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, sums_ptr[j]);
    }
  }

  return hash;
}
