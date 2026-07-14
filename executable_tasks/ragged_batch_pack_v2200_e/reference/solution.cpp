#include "interface.h"

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
      const std::size_t row = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      for (int i = 0; i < lengths[static_cast<std::size_t>(b)]; ++i) {
        hash = mix(hash, padded_tokens[row + static_cast<std::size_t>(i)]);
        hash = mix(hash, static_cast<uint64_t>(i));
      }
    }
  }
  return hash;
}
