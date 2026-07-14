#include "interface.h"
#include <cstdint>
#include <vector>

using std::uint64_t;
using std::vector;

static inline uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

uint64_t dictionary_decode_checksum(const vector<std::string>& dict,
                                     const vector<uint32_t>& codes,
                                     int iters) {
  uint64_t hash = 0;
  for (int i = 0; i < iters; ++i) {
    hash = 14695981039346656037ULL; // FNV-1a initial value
    for (uint32_t code : codes) {
      const std::string& token = dict[code];
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0 : static_cast<uint8_t>(token[0]));
    }
  }
  return hash;
}