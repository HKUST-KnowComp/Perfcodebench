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
  uint64_t hash = 0;
  
  // Pre-calculate the hash for a single pass to avoid redundant work if iters > 1
  // However, the problem implies the hash is reset per iteration in the baseline.
  // Looking at the baseline: hash is reset to the FNV offset basis inside the 'iters' loop.
  // Therefore, the final result is simply the result of the last iteration.
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::string& token = dict[static_cast<std::size_t>(code)];
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  
  return hash;
}
