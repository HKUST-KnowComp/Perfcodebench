#include "interface.h"
#include <vector>
#include <cstddef>

namespace {
  inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
  }
}

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  const std::size_t B = static_cast<std::size_t>(batch);
  const std::size_t L = static_cast<std::size_t>(max_len);

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t b = 0; b < B; ++b) {
      const std::size_t valid_len = static_cast<std::size_t>(lengths[b]);
      for (std::size_t pos = 0; pos < valid_len; ++pos) {
        const uint32_t token = padded_tokens[b * L + pos];
        hash = mix(hash, static_cast<uint64_t>(token));
        hash = mix(hash, static_cast<uint64_t>(pos));
      }
    }
  }
  return hash;
}