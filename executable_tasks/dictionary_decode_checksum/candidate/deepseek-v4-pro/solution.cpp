#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct TokenInfo {
  uint32_t len;
  unsigned char first;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute token lengths and first bytes.
  std::vector<TokenInfo> token_info;
  token_info.reserve(dict.size());
  for (const auto& s : dict) {
    token_info.push_back({
      static_cast<uint32_t>(s.size()),
      s.empty() ? 0 : static_cast<unsigned char>(s[0])
    });
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const auto& info = token_info[code];
      hash = mix(hash, info.len);
      hash = mix(hash, info.first);
    }
  }
  return hash;
}