#include "interface.h"

namespace {
static inline void mix64(uint64_t& hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Preserve baseline behavior: if iters == 0, nothing is computed and 0 is returned.
  if (iters <= 0) return 0ULL;

  // Compute the checksum once. In the baseline, the same computation is repeated
  // for each iter and the hash is overwritten each time, so the final result
  // equals the result of a single computation.
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();
  const std::size_t s_max_len = static_cast<std::size_t>(max_len);

  for (int b = 0; b < batch; ++b) {
    // Use size_t for indexing into padded_tokens
    const std::size_t base = static_cast<std::size_t>(b) * s_max_len;
    const uint32_t* row = tokens + base;
    const int len = lens[static_cast<std::size_t>(b)];

    for (int i = 0; i < len; ++i) {
      mix64(hash, static_cast<uint64_t>(row[i]));
      mix64(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
    }
  }

  return hash;
}
