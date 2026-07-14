#include "interface.h"

#include <cstdint>
#include <string>
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
  const std::size_t n = codes.size();
  const std::string* dict_data = dict.data();
  const uint32_t* codes_data = codes.data();

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = hash;
    std::size_t i = 0;
    // unroll 4×
    for (; i + 3 < n; i += 4) {
      const std::string& s0 = dict_data[codes_data[i]];
      const std::string& s1 = dict_data[codes_data[i + 1]];
      const std::string& s2 = dict_data[codes_data[i + 2]];
      const std::string& s3 = dict_data[codes_data[i + 3]];
      h = mix(h, s0.size());
      h = mix(h, s0.empty() ? 0u : static_cast<unsigned char>(s0[0]));
      h = mix(h, s1.size());
      h = mix(h, s1.empty() ? 0u : static_cast<unsigned char>(s1[0]));
      h = mix(h, s2.size());
      h = mix(h, s2.empty() ? 0u : static_cast<unsigned char>(s2[0]));
      h = mix(h, s3.size());
      h = mix(h, s3.empty() ? 0u : static_cast<unsigned char>(s3[0]));
    }
    // tail
    for (; i < n; ++i) {
      const std::string& s = dict_data[codes_data[i]];
      h = mix(h, s.size());
      h = mix(h, s.empty() ? 0u : static_cast<unsigned char>(s[0]));
    }
    hash = h;
  }
  return hash;
}