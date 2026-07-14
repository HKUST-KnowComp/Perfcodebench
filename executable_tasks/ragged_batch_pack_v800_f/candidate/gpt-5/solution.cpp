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
  uint64_t hash = 0;
  const uint32_t* tokens = padded_tokens.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      int len = lengths[static_cast<std::size_t>(b)];
      const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      int i = 0;
      // Unrolled loop to reduce overhead on very long sequences
      for (; i + 3 < len; i += 4) {
        h = mix(h, row[i]);
        h = mix(h, static_cast<uint32_t>(i));
        h = mix(h, row[i + 1]);
        h = mix(h, static_cast<uint32_t>(i + 1));
        h = mix(h, row[i + 2]);
        h = mix(h, static_cast<uint32_t>(i + 2));
        h = mix(h, row[i + 3]);
        h = mix(h, static_cast<uint32_t>(i + 3));
      }
      for (; i < len; ++i) {
        h = mix(h, row[i]);
        h = mix(h, static_cast<uint32_t>(i));
      }
    }
    hash = h;
  }
  return hash;
}
