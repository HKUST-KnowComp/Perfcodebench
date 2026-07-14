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
  size_t total_tokens = 0;
  for (int len : lengths) {
    total_tokens += len;
  }

  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_tokens);
  positions.reserve(total_tokens);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    for (int b = 0; b < batch; ++b) {
      for (int i = 0; i < lengths[b]; ++i) {
        packed_tokens.push_back(padded_tokens[b * max_len + i]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packed_tokens.size(); ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}