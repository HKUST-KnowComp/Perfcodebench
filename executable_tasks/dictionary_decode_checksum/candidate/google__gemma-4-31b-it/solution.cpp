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
  
  // Pre-calculate the sequence of lengths and first bytes to avoid repeated dict access
  // and string overhead inside the iters loop.
  struct TokenInfo {
    uint64_t size;
    uint64_t first_byte;
  };
  
  std::vector<TokenInfo> precomputed;
  precomputed.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& s = dict[static_cast<std::size_t>(code)];
    precomputed.push_back({static_cast<uint64_t>(s.size()), 
                           s.empty() ? 0u : static_cast<unsigned char>(s[0])});
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& token : precomputed) {
      hash = mix(hash, token.size);
      hash = mix(hash, token.first_byte);
    }
    final_hash = hash;
  }
  
  return final_hash;
}