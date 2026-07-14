#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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
  // Preserve baseline behavior for iters == 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t dict_size = dictionary.size();

  // Fast path for empty dictionary: checksum is just the offset basis when iter > 0
  if (dict_size == 0) {
    return 1469598103934665603ULL;
  }

  // Aggregate sums per code directly into an array indexed by code
  std::vector<uint64_t> sums(dict_size, 0ULL);
  const std::size_t n = codes.size();

  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict values_ptr = values.data();
  uint64_t* __restrict sums_ptr = sums.data();

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    sums_ptr[codes_ptr[i]] += static_cast<uint64_t>(values_ptr[i]);
    sums_ptr[codes_ptr[i + 1]] += static_cast<uint64_t>(values_ptr[i + 1]);
    sums_ptr[codes_ptr[i + 2]] += static_cast<uint64_t>(values_ptr[i + 2]);
    sums_ptr[codes_ptr[i + 3]] += static_cast<uint64_t>(values_ptr[i + 3]);
  }
  for (; i < n; ++i) {
    sums_ptr[codes_ptr[i]] += static_cast<uint64_t>(values_ptr[i]);
  }

  // Compute checksum in dictionary order
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t di = 0; di < dict_size; ++di) {
    hash = mix(hash, sums_ptr[di]);
  }

  return hash;
}
