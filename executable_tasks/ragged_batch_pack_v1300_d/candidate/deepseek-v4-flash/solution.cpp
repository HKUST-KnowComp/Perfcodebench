#include "interface.h"
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  const auto* tokens_ptr = padded_tokens.data();
  const auto* lengths_ptr = lengths.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      const int len = lengths_ptr[b];
      const size_t base = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        const uint32_t token = tokens_ptr[base + static_cast<size_t>(i)];
        hash ^= token;
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint32_t>(i);
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}
