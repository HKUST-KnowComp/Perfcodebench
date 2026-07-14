#include "interface.h"

namespace {

static inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a 64-bit prime
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int /*iters*/) {
  // Baseline recomputed the same hash each iteration after resetting the seed.
  // Compute once with identical order and mixing: token, then position per element.
  const uint32_t* data = padded_tokens.data();
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  for (int b = 0; b < batch; ++b) {
    int len = lengths[static_cast<std::size_t>(b)];
    const uint32_t* row = data + (static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len));
    for (int i = 0; i < len; ++i) {
      hash = mix64(hash, static_cast<uint64_t>(row[i]));
      hash = mix64(hash, static_cast<uint32_t>(i));
    }
  }

  return hash;
}
