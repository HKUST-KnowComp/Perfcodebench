#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const size_t dict_size = dict.size();
  std::vector<uint64_t> dict_lens(dict_size);
  std::vector<uint8_t> dict_firsts(dict_size);
  
  for (size_t i = 0; i < dict_size; ++i) {
    dict_lens[i] = dict[i].size();
    dict_firsts[i] = dict_lens[i] == 0 ? 0 : static_cast<uint8_t>(dict[i][0]);
  }

  const uint64_t* lens_ptr = dict_lens.data();
  const uint8_t* firsts_ptr = dict_firsts.data();
  const uint32_t* codes_ptr = codes.data();
  const size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < codes_size; ++i) {
      uint32_t code = codes_ptr[i];
      hash ^= lens_ptr[code];
      hash *= 1099511628211ULL;
      hash ^= firsts_ptr[code];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}