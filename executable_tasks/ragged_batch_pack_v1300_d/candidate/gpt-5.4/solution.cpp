#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t batch_sz = static_cast<std::size_t>(batch);
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);

  std::size_t total_tokens = 0;
  for (std::size_t b = 0; b < batch_sz; ++b) {
    total_tokens += static_cast<std::size_t>(lengths[b]);
  }

  std::vector<uint32_t> positions(total_tokens);
  {
    std::size_t out = 0;
    for (std::size_t b = 0; b < batch_sz; ++b) {
      const uint32_t len = static_cast<uint32_t>(lengths[b]);
      for (uint32_t i = 0; i < len; ++i) {
        positions[out++] = i;
      }
    }
  }

  std::vector<uint32_t> packed_tokens(total_tokens);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t out = 0;
    for (std::size_t b = 0; b < batch_sz; ++b) {
      const std::size_t len = static_cast<std::size_t>(lengths[b]);
      const uint32_t* src = padded_tokens.data() + b * max_len_sz;
      for (std::size_t i = 0; i < len; ++i) {
        packed_tokens[out++] = src[i];
      }
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_tokens; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}
