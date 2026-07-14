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

  std::size_t total_packed = 0;
  for (std::size_t b = 0; b < batch_sz; ++b) {
    total_packed += static_cast<std::size_t>(lengths[b]);
  }

  std::vector<uint32_t> packed_tokens(total_packed);
  std::vector<uint32_t> positions(total_packed);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t out = 0;
    for (std::size_t b = 0; b < batch_sz; ++b) {
      const int len_i = lengths[b];
      const std::size_t len = static_cast<std::size_t>(len_i);
      const std::size_t base = b * max_len_sz;

      for (std::size_t i = 0; i < len; ++i) {
        packed_tokens[out] = padded_tokens[base + i];
        positions[out] = static_cast<uint32_t>(i);
        ++out;
      }
    }

    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_packed; ++i) {
      h = mix(h, packed_tokens[i]);
      h = mix(h, positions[i]);
    }
    hash = h;
  }
  return hash;
}
