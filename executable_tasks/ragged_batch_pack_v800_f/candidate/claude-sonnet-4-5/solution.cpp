#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total tokens to avoid reallocation
  std::size_t total_tokens = 0;
  for (int b = 0; b < batch; ++b) {
    total_tokens += static_cast<std::size_t>(lengths[b]);
  }
  
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_tokens);
  positions.reserve(total_tokens);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < batch; ++b) {
      int len = lengths[b];
      std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        uint32_t token = padded_tokens[base + static_cast<std::size_t>(i)];
        uint32_t pos = static_cast<uint32_t>(i);
        packed_tokens.push_back(token);
        positions.push_back(pos);
        hash = mix(hash, token);
        hash = mix(hash, pos);
      }
    }
  }
  return hash;
}
