#include "interface.h"

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
  uint64_t final_hash = 0;
  
  // Pre-calculate the values to mix to avoid repeated string access and size calls
  // since the dictionary and codes are constant across iterations.
  struct TokenInfo {
    size_t size;
    unsigned char first_char;
  };
  
  std::vector<TokenInfo> precomputed;
  precomputed.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& s = dict[static_cast<std::size_t>(code)];
    precomputed.push_back({s.size(), s.empty() ? 0u : static_cast<unsigned char>(s[0])});
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& token : precomputed) {
      hash = mix(hash, token.size);
      hash = mix(hash, token.first_char);
    }
    final_hash = hash;
  }
  
  return final_hash;
}