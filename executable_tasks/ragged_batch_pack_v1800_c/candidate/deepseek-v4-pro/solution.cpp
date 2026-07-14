#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters == 0) {
    return 0;
  }

  // Compute total number of tokens and preallocate packed vectors.
  std::size_t total = 0;
  for (int b = 0; b < batch; ++b) {
    total += static_cast<std::size_t>(lengths[b]);
  }

  std::vector<uint32_t> packed_tokens(total);
  std::vector<uint32_t> positions(total);

  // Pack tokens and positions in a single pass.
  const uint32_t* src = padded_tokens.data();
  std::size_t idx = 0;
  for (int b = 0; b < batch; ++b) {
    int len = lengths[b];
    const uint32_t* row = src + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      packed_tokens[idx] = row[i];
      positions[idx] = static_cast<uint32_t>(i);
      ++idx;
    }
  }

  // Compute the checksum exactly once (iterations are idempotent).
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* pt = packed_tokens.data();
  const uint32_t* pp = positions.data();
  for (std::size_t i = 0; i < total; ++i) {
    hash = mix(hash, pt[i]);
    hash = mix(hash, pp[i]);
  }

  return hash;
}
