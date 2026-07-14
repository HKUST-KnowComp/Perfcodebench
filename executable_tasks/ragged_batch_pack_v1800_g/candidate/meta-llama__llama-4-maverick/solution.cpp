#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  uint64_t hash = 0;
  std::size_t total_length = 0;
  for (int b = 0; b < batch; ++b) {
    total_length += static_cast<std::size_t>(lengths[b]);
  }
  std::vector<uint32_t> packed_tokens(total_length);
  std::vector<uint32_t> positions(total_length);
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t idx = 0;
    for (int b = 0; b < batch; ++b) {
      for (int i = 0; i < lengths[static_cast<std::size_t>(b)]; ++i) {
        packed_tokens[idx] = padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len) + static_cast<std::size_t>(i)];
        positions[idx] = static_cast<uint32_t>(i);
        ++idx;
      }
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_length; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}
