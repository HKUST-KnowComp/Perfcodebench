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
  uint64_t hash = 0;
  
  // Pre-calculate values to avoid repeated indexing and method calls inside the loop
  struct TokenInfo {
    uint64_t size;
    uint64_t first_char;
  };
  
  std::vector<TokenInfo> info;
  info.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& s = dict[static_cast<std::size_t>(code)];
    info.push_back({static_cast<uint64_t>(s.size()), 
                    s.empty() ? 0u : static_cast<unsigned char>(s[0])});
  }

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const auto& token : info) {
      hash = mix(hash, token.size);
      hash = mix(hash, token.first_char);
    }
  }
  
  return hash;
}