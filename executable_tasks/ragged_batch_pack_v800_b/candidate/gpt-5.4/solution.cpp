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

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t b = 0; b < batch_sz; ++b) {
      const int len_i = lengths[b];
      const std::size_t len = static_cast<std::size_t>(len_i);
      const uint32_t* row = padded_tokens.data() + b * max_len_sz;

      std::size_t i = 0;
      for (; i + 4 <= len; i += 4) {
        h = mix(h, row[i + 0]);
        h = mix(h, static_cast<uint64_t>(i + 0));
        h = mix(h, row[i + 1]);
        h = mix(h, static_cast<uint64_t>(i + 1));
        h = mix(h, row[i + 2]);
        h = mix(h, static_cast<uint64_t>(i + 2));
        h = mix(h, row[i + 3]);
        h = mix(h, static_cast<uint64_t>(i + 3));
      }
      for (; i < len; ++i) {
        h = mix(h, row[i]);
        h = mix(h, static_cast<uint64_t>(i));
      }
    }
    hash = h;
  }

  (void)total_tokens;
  return hash;
}
