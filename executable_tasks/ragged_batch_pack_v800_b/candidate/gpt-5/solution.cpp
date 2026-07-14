#include "interface.h"

#include <vector>

namespace {
inline uint64_t mix_inline(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  const uint32_t* padded = padded_tokens.data();
  const int* lens = lengths.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      int len = lens[static_cast<std::size_t>(b)];
      const uint32_t* base = padded + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        // Mix token
        h ^= static_cast<uint64_t>(base[static_cast<std::size_t>(i)]);
        h *= 1099511628211ULL;
        // Mix position within the sequence row
        h ^= static_cast<uint64_t>(static_cast<uint32_t>(i));
        h *= 1099511628211ULL;
      }
    }
    hash = h;
  }
  return hash;
}
