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
  const uint32_t* const tokens = padded_tokens.data();
  const int* const lens = lengths.data();
  const std::size_t stride = static_cast<std::size_t>(max_len);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    for (int b = 0; b < batch; ++b) {
      const int len = lens[b];
      const uint32_t* row = tokens + static_cast<std::size_t>(b) * stride;
      for (int i = 0; i < len; ++i) {
        h = mix(h, row[i]);
        h = mix(h, static_cast<uint32_t>(i));
      }
    }

    hash = h;
  }
  return hash;
}
