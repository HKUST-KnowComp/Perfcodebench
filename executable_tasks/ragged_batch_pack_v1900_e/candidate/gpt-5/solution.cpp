#include "interface.h"

#include <vector>
#include <cstdint>

namespace {
static inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Baseline resets hash each iteration and returns the final iteration's hash.
  // Therefore, for iters <= 0, baseline returns 0; otherwise the result is the same for all iterations.
  if (iters < 1) {
    return 0ULL;
  }

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  for (int b = 0; b < batch; ++b) {
    int len = lengths[static_cast<std::size_t>(b)];
    const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    const uint32_t* row = padded_tokens.data() + base;
    for (int i = 0; i < len; ++i) {
      // Token first, then position, matching the original order.
      hash = mix64(hash, static_cast<uint64_t>(row[i]));
      hash = mix64(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
    }
  }

  return hash;
}
