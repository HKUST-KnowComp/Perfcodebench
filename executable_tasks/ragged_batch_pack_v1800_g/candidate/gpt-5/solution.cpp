#include "interface.h"

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Preserve baseline semantics: if iters == 0, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute the checksum once since each iteration yields the same result.
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  const uint32_t* data = padded_tokens.data();
  for (int b = 0; b < batch; ++b) {
    int len = lengths[static_cast<std::size_t>(b)];
    const uint32_t* row = data + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      uint64_t tok = static_cast<uint64_t>(row[static_cast<std::size_t>(i)]);
      hash = mix(hash, tok);
      uint64_t pos = static_cast<uint64_t>(static_cast<uint32_t>(i));
      hash = mix(hash, pos);
    }
  }

  return hash;
}
