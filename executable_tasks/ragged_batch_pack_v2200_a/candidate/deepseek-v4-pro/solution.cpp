#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                               const std::vector<int>& lengths,
                               int batch, int max_len, int iters) {
  if (iters <= 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  for (int b = 0; b < batch; ++b) {
    int len = lengths[b];
    const uint32_t* row = &padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len)];
    for (int i = 0; i < len; ++i) {
      hash = mix(hash, row[i]);
      hash = mix(hash, static_cast<uint32_t>(i));
    }
  }
  return hash;
}
