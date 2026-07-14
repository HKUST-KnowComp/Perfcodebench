#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  const size_t dict_size = dict.size();
  std::vector<uint64_t> sizes(dict_size);
  std::vector<uint8_t> first_bytes(dict_size);
  
  for (size_t i = 0; i < dict_size; ++i) {
    sizes[i] = static_cast<uint64_t>(dict[i].size());
    first_bytes[i] = dict[i].empty() ? 0u : static_cast<uint8_t>(dict[i][0]);
  }

  const uint32_t* codes_ptr = codes.data();
  const size_t codes_count = codes.size();
  const uint64_t* sizes_ptr = sizes.data();
  const uint8_t* first_ptr = first_bytes.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < codes_count; ++i) {
      uint32_t c = codes_ptr[i];
      hash = mix(hash, sizes_ptr[c]);
      hash = mix(hash, first_ptr[c]);
    }
  }
  return hash;
}