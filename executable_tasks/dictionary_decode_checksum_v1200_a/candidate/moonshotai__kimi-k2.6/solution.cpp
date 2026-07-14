#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct TokenMeta {
  uint64_t size;
  uint64_t first;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t dict_size = dict.size();
  std::vector<TokenMeta> meta(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    meta[i].size = static_cast<uint64_t>(s.size());
    meta[i].first = s.empty() ? 0u : static_cast<unsigned char>(s[0]);
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t code : codes) {
    const std::size_t idx = static_cast<std::size_t>(code);
    const TokenMeta& m = meta[idx];
    hash = mix(hash, m.size);
    hash = mix(hash, m.first);
  }

  return hash;
}
