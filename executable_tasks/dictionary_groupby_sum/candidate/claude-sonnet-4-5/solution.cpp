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
  uint64_t hash = 0;
  const std::size_t dict_size = dictionary.size();
  
  // Use stack allocation for small dictionaries, heap for large
  constexpr std::size_t STACK_THRESHOLD = 4096;
  uint64_t stack_sums[STACK_THRESHOLD];
  std::vector<uint64_t> heap_sums;
  uint64_t* sums;
  
  if (dict_size <= STACK_THRESHOLD) {
    sums = stack_sums;
  } else {
    heap_sums.resize(dict_size);
    sums = heap_sums.data();
  }
  
  const std::size_t n = codes.size();
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* values_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear sums array
    std::memset(sums, 0, dict_size * sizeof(uint64_t));
    
    // Direct aggregation using codes as indices
    for (std::size_t i = 0; i < n; ++i) {
      sums[codes_ptr[i]] += values_ptr[i];
    }
    
    // Compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  
  return hash;
}