#include "interface.h"

#include <vector>
#include <algorithm>
#include <cstddef>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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

  // Prepare sums buffer once; reuse across iterations.
  std::vector<uint64_t> sums(dict_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums for this iteration
    if (dict_size) {
      std::fill(sums.begin(), sums.end(), 0ULL);
    }

    // Accumulate sums by dictionary code
    const uint16_t* cptr = codes.data();
    const uint32_t* vptr = values.data();
    std::size_t i = 0;

    // Unrolled loop for better throughput
    const std::size_t unroll = 4;
    const std::size_t limit = n - (n % unroll);
    for (; i < limit; i += unroll) {
      sums[cptr[0]] += static_cast<uint64_t>(vptr[0]);
      sums[cptr[1]] += static_cast<uint64_t>(vptr[1]);
      sums[cptr[2]] += static_cast<uint64_t>(vptr[2]);
      sums[cptr[3]] += static_cast<uint64_t>(vptr[3]);
      cptr += unroll;
      vptr += unroll;
    }
    for (; i < n; ++i) {
      sums[*cptr++] += static_cast<uint64_t>(*vptr++);
    }

    // Compute hash in dictionary order
    hash = 1469598103934665603ULL; // FNV offset basis
    for (std::size_t k = 0; k < dict_size; ++k) {
      hash = mix(hash, sums[k]);
    }
  }

  return hash;
}
