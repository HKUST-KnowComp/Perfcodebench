#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  // Precompute lengths and first bytes once to avoid std::string overhead 
  // and pointer chasing during the hot loop.
  size_t dict_size = dict.size();
  std::vector<uint64_t> lens(dict_size);
  std::vector<uint8_t> firsts(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    lens[i] = dict[i].size();
    firsts[i] = dict[i].empty() ? 0 : static_cast<uint8_t>(dict[i][0]);
  }

  const uint64_t* lens_ptr = lens.data();
  const uint8_t* firsts_ptr = firsts.data();
  const uint32_t* codes_ptr = codes.data();
  size_t num_codes = codes.size();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_codes; ++i) {
      uint32_t code = codes_ptr[i];
      hash ^= lens_ptr[code];
      hash *= 1099511628211ULL;
      hash ^= firsts_ptr[code];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}