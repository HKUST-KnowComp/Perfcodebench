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
  
  // Pre-calculate the sequence of references or pointers if codes is large,
  // but since we just need the checksum, we can iterate directly.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::string& token = dict[static_cast<std::size_t>(code)];
      hash = mix(hash, static_cast<uint64_t>(token.size()));
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
    final_hash = hash;
  }
  return final_hash;
}