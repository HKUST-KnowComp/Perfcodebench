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
      const int len_i = lens[b];
      const std::size_t base = static_cast<std::size_t>(b) * stride;
      const uint32_t* row = tokens + base;

      int i = 0;
      for (; i + 7 < len_i; i += 8) {
        h = mix(h, row[i + 0]); h = mix(h, static_cast<uint32_t>(i + 0));
        h = mix(h, row[i + 1]); h = mix(h, static_cast<uint32_t>(i + 1));
        h = mix(h, row[i + 2]); h = mix(h, static_cast<uint32_t>(i + 2));
        h = mix(h, row[i + 3]); h = mix(h, static_cast<uint32_t>(i + 3));
        h = mix(h, row[i + 4]); h = mix(h, static_cast<uint32_t>(i + 4));
        h = mix(h, row[i + 5]); h = mix(h, static_cast<uint32_t>(i + 5));
        h = mix(h, row[i + 6]); h = mix(h, static_cast<uint32_t>(i + 6));
        h = mix(h, row[i + 7]); h = mix(h, static_cast<uint32_t>(i + 7));
      }
      for (; i < len_i; ++i) {
        h = mix(h, row[i]);
        h = mix(h, static_cast<uint32_t>(i));
      }
    }

    hash = h;
  }
  return hash;
}
