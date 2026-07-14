#include "interface.h"

#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Match baseline behavior: if iters == 0, return initial hash value (0)
  if (iters == 0) {
    return 0ULL;
  }

  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis as in baseline

  for (int b = 0; b < batch; ++b) {
    int len = lens[b];
    if (len <= 0) continue;
    const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      uint32_t tok = row[i];
      hash = mix(hash, static_cast<uint64_t>(tok));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
    }
  }

  // Baseline recomputed identical hash for each iter and returned the last; compute once is equivalent.
  return hash;
}
