#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) return 0ULL;
  uint64_t hash = 1469598103934665603ULL;
  for (int b = 0; b < batch; ++b) {
    int len = lengths[static_cast<size_t>(b)];
    size_t base = static_cast<size_t>(b) * static_cast<size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      uint64_t token = padded_tokens[base + static_cast<size_t>(i)];
      hash = mix(hash, token);
      hash = mix(hash, static_cast<uint64_t>(i));
    }
  }
  return hash;
}