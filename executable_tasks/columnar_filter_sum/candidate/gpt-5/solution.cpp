#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;
    const std::size_t limit = n & static_cast<std::size_t>(~7ULL); // process in chunks of 8

    for (; i < limit; i += 8) {
      // 0
      {
        uint32_t key = k[i + 0];
        uint32_t m = (f[i + 0] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 0] & (0u - m));
      }
      // 1
      {
        uint32_t key = k[i + 1];
        uint32_t m = (f[i + 1] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 1] & (0u - m));
      }
      // 2
      {
        uint32_t key = k[i + 2];
        uint32_t m = (f[i + 2] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 2] & (0u - m));
      }
      // 3
      {
        uint32_t key = k[i + 3];
        uint32_t m = (f[i + 3] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 3] & (0u - m));
      }
      // 4
      {
        uint32_t key = k[i + 4];
        uint32_t m = (f[i + 4] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 4] & (0u - m));
      }
      // 5
      {
        uint32_t key = k[i + 5];
        uint32_t m = (f[i + 5] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 5] & (0u - m));
      }
      // 6
      {
        uint32_t key = k[i + 6];
        uint32_t m = (f[i + 6] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 6] & (0u - m));
      }
      // 7
      {
        uint32_t key = k[i + 7];
        uint32_t m = (f[i + 7] != 0);
        m &= (key < key_threshold);
        m &= ((key & 3u) == 0u);
        s += static_cast<uint64_t>(v[i + 7] & (0u - m));
      }
    }

    for (; i < n; ++i) {
      uint32_t key = k[i];
      uint32_t m = (f[i] != 0);
      m &= (key < key_threshold);
      m &= ((key & 3u) == 0u);
      s += static_cast<uint64_t>(v[i] & (0u - m));
    }

    sum = s; // per baseline semantics, the final iteration's sum is returned
  }

  return sum;
}
