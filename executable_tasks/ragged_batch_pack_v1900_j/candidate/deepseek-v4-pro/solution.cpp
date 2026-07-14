#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  const uint32_t* base = padded_tokens.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      const uint32_t* row = base + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      int len = lengths[static_cast<std::size_t>(b)];
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, row[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  return hash;
}
