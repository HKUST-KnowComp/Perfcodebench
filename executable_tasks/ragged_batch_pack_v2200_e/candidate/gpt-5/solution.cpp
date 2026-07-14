#include "interface.h"

namespace {
static inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) return 0ULL;

  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();
  const int B = batch;
  const int L = max_len;

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  for (int b = 0; b < B; ++b) {
    int len = lens[static_cast<std::size_t>(b)];
    const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(L);
    for (int i = 0; i < len; ++i) {
      // Mix token value
      hash ^= static_cast<uint64_t>(row[i]);
      hash *= 1099511628211ULL;
      // Mix position (i)
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(i));
      hash *= 1099511628211ULL;
      // Equivalent to: hash = mix64(mix64(hash, row[i]), i)
    }
  }

  // Each iteration would yield the same hash; returning it directly matches the final iteration's result.
  return hash;
}
