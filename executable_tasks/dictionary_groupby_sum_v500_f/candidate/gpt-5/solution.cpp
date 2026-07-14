#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  uint64_t hash = 0;
  std::vector<uint64_t> sums(dict_size);

  for (int iter = 0; iter < iters; ++iter) {
    // Zero sums for this iteration
    if (dict_size != 0) {
      std::fill(sums.begin(), sums.end(), 0ULL);
    }

    // Accumulate per-code sums
    const uint16_t* cptr = codes.data();
    const uint32_t* vptr = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      sums[static_cast<std::size_t>(cptr[i])] += static_cast<uint64_t>(vptr[i]);
    }

    // Compute FNV-1a hash over sums in dictionary order
    uint64_t h = 1469598103934665603ULL; // FNV offset basis
    const uint64_t fnv_prime = 1099511628211ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      h ^= sums[i];
      h *= fnv_prime;
    }
    hash = h;
  }

  return hash;
}
