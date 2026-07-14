#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t fast_mix(uint64_t h, uint64_t v) noexcept {
  h ^= v;
  h ^= h >> 32;
  h *= 0x9e3779b97f4a7c15ULL;
  h ^= h >> 32;
  return h;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  const uint32_t batch_u   = static_cast<uint32_t>(batch);
  const uint32_t max_len_u = static_cast<uint32_t>(max_len);

  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;

  uint32_t total = 0;
  for (uint32_t b = 0; b < batch_u; ++b) {
    total += static_cast<uint32_t>(lengths[b]);
  }
  packed_tokens.reserve(total);
  positions.reserve(total);

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();

    const uint32_t* src = padded_tokens.data();
    for (uint32_t b = 0; b < batch_u; ++b) {
      const uint32_t len = static_cast<uint32_t>(lengths[b]);
      const uint32_t row_offset = b * max_len_u;
      for (uint32_t i = 0; i < len; ++i) {
        const uint32_t tok = src[row_offset + i];
        packed_tokens.push_back(tok);
        positions.push_back(i);
      }
    }

    hash = 1469598103934665603ULL;
    const uint32_t* tok_ptr = packed_tokens.data();
    const uint32_t* pos_ptr = positions.data();
    const uint32_t N = static_cast<uint32_t>(packed_tokens.size());
    for (uint32_t i = 0; i < N; ++i) {
      hash = fast_mix(hash, tok_ptr[i]);
      hash = fast_mix(hash, pos_ptr[i]);
    }
  }
  return hash;
}