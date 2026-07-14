#include "interface.h"

namespace {

static inline uint64_t mix_u64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int /*iters*/) {
  const uint32_t* base = padded_tokens.data();
  const std::size_t stride = static_cast<std::size_t>(max_len);

  uint64_t hash = 1469598103934665603ULL;

  for (int b = 0; b < batch; ++b) {
    const uint32_t* row = base + static_cast<std::size_t>(b) * stride;
    int len = lengths[static_cast<std::size_t>(b)];
    if (len <= 0) continue;
    for (int i = 0; i < len; ++i) {
      hash = mix_u64(hash, static_cast<uint64_t>(row[i]));
      hash = mix_u64(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
    }
  }

  return hash;
}
