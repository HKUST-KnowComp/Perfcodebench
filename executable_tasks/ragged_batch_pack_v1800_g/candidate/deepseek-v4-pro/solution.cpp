#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int /*iters*/) {
  uint64_t hash = 1469598103934665603ULL;
  std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  for (int b = 0; b < batch; ++b) {
    std::size_t base = static_cast<std::size_t>(b) * max_len_sz;
    int len = lengths[b];
    for (int i = 0; i < len; ++i) {
      uint64_t token = static_cast<uint64_t>(padded_tokens[base + i]);
      hash = mix(hash, token);
      hash = mix(hash, static_cast<uint64_t>(i));
    }
  }
  return hash;
}