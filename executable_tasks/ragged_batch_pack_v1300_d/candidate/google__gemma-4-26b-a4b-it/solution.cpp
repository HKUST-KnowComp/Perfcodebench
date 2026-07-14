#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) return 0;

  // Pre-calculate total tokens to avoid re-allocations and use a single buffer
  size_t total_tokens = 0;
  for (int b = 0; b < batch; ++b) {
    total_tokens += lengths[static_cast<size_t>(b)];
  }

  // Pre-allocate a buffer for packed tokens to avoid push_back overhead
  std::vector<uint32_t> packed_tokens;
  packed_tokens.reserve(total_tokens);

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    
    // Step 1: Pack tokens
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(padded_tokens[offset + i]);
      }
    }

    // Step 2: Hash tokens and positions
    // We re-calculate positions on the fly to avoid the memory overhead of a 'positions' vector
    hash = FNV_OFFSET_BASIS;
    size_t current_idx = 0;
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        // The original logic: hash = mix(hash, token); hash = mix(hash, position);
        hash = mix(hash, static_cast<uint64_t>(packed_tokens[current_idx++]));
        hash = mix(hash, static_cast<uint64_t>(i));
      }
    }
  }

  return hash;
}
