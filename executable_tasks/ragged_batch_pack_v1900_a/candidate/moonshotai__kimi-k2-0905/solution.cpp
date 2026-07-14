#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  // Pre-compute total number of valid tokens across the whole batch
  int total_tokens = 0;
  for (int b = 0; b < batch; ++b) total_tokens += lengths[static_cast<size_t>(b)];

  // One-time allocation for the entire benchmark
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.resize(static_cast<size_t>(total_tokens));
  positions.resize(static_cast<size_t>(total_tokens));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* src = padded_tokens.data();
    uint32_t* dst_tok = packed_tokens.data();
    uint32_t* dst_pos = positions.data();

    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      for (int pos = 0; pos < len; ++pos) {
        *dst_tok++ = src[pos];
        *dst_pos++ = static_cast<uint32_t>(pos);
      }
      src += max_len;
    }

    hash = 1469598103934665603ULL;
    const uint32_t* tok = packed_tokens.data();
    const uint32_t* pos = positions.data();
    for (int i = 0; i < total_tokens; ++i) {
      hash = mix(hash, tok[i]);
      hash = mix(hash, pos[i]);
    }
  }
  return hash;
}