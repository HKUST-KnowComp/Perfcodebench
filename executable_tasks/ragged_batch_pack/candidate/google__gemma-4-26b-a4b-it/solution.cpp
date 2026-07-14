#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  uint64_t hash = 0;
  
  // Pre-calculate total elements to avoid vector overhead if we were using them,
  // but since we are optimizing for performance, we skip the vectors entirely
  // and compute the hash in a single pass to maximize cache locality and minimize memory traffic.
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<std::size_t>(b)];
      const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      
      for (int i = 0; i < len; ++i) {
        uint32_t token = padded_tokens[offset + i];
        uint32_t pos = static_cast<uint32_t>(i);
        
        hash = mix(hash, static_cast<uint64_t>(token));
        hash = mix(hash, static_cast<uint64_t>(pos));
      }
    }
  }
  
  return hash;
}
