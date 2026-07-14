#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  // Pre-compute total number of tokens to pack
  std::size_t total = 0;
  for (int b = 0; b < batch; ++b) total += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);

  // Single allocation, done once
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total);
  positions.reserve(total);

  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();

    const uint32_t* row = padded_tokens.data();
    for (int b = 0; b < batch; ++b) {
      int len = lengths[static_cast<std::size_t>(b)];
      for (int i = 0; i < len; ++i) {
        uint32_t tok = row[i];
        packed_tokens.push_back(tok);
        positions.push_back(static_cast<uint32_t>(i));
      }
      row += max_len;
    }

    hash = 1469598103934665603ULL;
    const std::size_t n = packed_tokens.size();
    const uint32_t* ptok = packed_tokens.data();
    const uint32_t* ppos = positions.data();
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, ptok[i]);
      hash = mix(hash, ppos[i]);
    }
  }
  return hash;
}