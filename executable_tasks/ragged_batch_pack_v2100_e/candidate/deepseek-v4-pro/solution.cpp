#include "interface.h"

#include <vector>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      int L = lengths[b];
      const uint32_t* row = padded_tokens.data() + static_cast<std::size_t>(b) * max_len;
      for (int i = 0; i < L; ++i) {
        hash = mix(hash, row[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  return hash;
}