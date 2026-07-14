#include "interface.h"

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* const tokens = padded_tokens.data();
  const int* const lens = lengths.data();
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);

  for (int b = 0; b < batch; ++b) {
    const int len = lens[b];
    const uint32_t* row = tokens + static_cast<std::size_t>(b) * max_len_sz;
    for (int i = 0; i < len; ++i) {
      hash ^= static_cast<uint64_t>(row[i]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(i);
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}