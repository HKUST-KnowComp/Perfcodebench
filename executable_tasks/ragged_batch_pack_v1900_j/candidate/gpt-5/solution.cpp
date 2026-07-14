#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  const uint32_t* data = padded_tokens.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      int len = lengths[static_cast<std::size_t>(b)];
      if (len <= 0) continue;
      const uint32_t* base = data + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);

      int i = 0;
      // Unrolled processing by 4 for better throughput
      for (; i + 4 <= len; i += 4) {
        uint32_t t0 = base[i];
        uint32_t t1 = base[i + 1];
        uint32_t t2 = base[i + 2];
        uint32_t t3 = base[i + 3];
        h = mix(h, static_cast<uint64_t>(t0));
        h = mix(h, static_cast<uint64_t>(static_cast<uint32_t>(i)));
        h = mix(h, static_cast<uint64_t>(t1));
        h = mix(h, static_cast<uint64_t>(static_cast<uint32_t>(i + 1)));
        h = mix(h, static_cast<uint64_t>(t2));
        h = mix(h, static_cast<uint64_t>(static_cast<uint32_t>(i + 2)));
        h = mix(h, static_cast<uint64_t>(t3));
        h = mix(h, static_cast<uint64_t>(static_cast<uint32_t>(i + 3)));
      }
      for (; i < len; ++i) {
        h = mix(h, static_cast<uint64_t>(base[i]));
        h = mix(h, static_cast<uint64_t>(static_cast<uint32_t>(i)));
      }
    }
    hash = h;  // Match baseline behavior: overwrite each iteration
  }

  return hash;
}
