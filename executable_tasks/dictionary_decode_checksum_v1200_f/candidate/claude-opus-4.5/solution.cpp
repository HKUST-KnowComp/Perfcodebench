#include "interface.h"

#include <cstdint>
#include <vector>

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
  
  // Precompute (size, first_byte) for each dictionary entry
  const size_t dict_size = dict.size();
  std::vector<std::pair<uint64_t, uint64_t>> precomputed(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    precomputed[i].first = s.size();
    precomputed[i].second = s.empty() ? 0ULL : static_cast<unsigned char>(s[0]);
  }
  
  const std::pair<uint64_t, uint64_t>* __restrict__ pre_ptr = precomputed.data();
  const uint32_t* __restrict__ codes_ptr = codes.data();
  const size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < codes_size; ++i) {
      const auto& p = pre_ptr[codes_ptr[i]];
      hash ^= p.first;
      hash *= 1099511628211ULL;
      hash ^= p.second;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}