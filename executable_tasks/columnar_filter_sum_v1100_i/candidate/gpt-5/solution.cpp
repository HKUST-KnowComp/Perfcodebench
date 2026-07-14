#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
  std::size_t i = 0;

  for (; i + 4 <= n; i += 4) {
    uint32_t k0 = k[i];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];

    uint64_t m0 = 0ull - static_cast<uint64_t>(((f[i] != 0) & (k0 < key_threshold) & ((k0 & 3u) == 0u)));
    uint64_t m1 = 0ull - static_cast<uint64_t>(((f[i + 1] != 0) & (k1 < key_threshold) & ((k1 & 3u) == 0u)));
    uint64_t m2 = 0ull - static_cast<uint64_t>(((f[i + 2] != 0) & (k2 < key_threshold) & ((k2 & 3u) == 0u)));
    uint64_t m3 = 0ull - static_cast<uint64_t>(((f[i + 3] != 0) & (k3 < key_threshold) & ((k3 & 3u) == 0u)));

    acc0 += (static_cast<uint64_t>(v[i])     & m0);
    acc1 += (static_cast<uint64_t>(v[i + 1]) & m1);
    acc2 += (static_cast<uint64_t>(v[i + 2]) & m2);
    acc3 += (static_cast<uint64_t>(v[i + 3]) & m3);
  }

  for (; i < n; ++i) {
    uint32_t kk = k[i];
    uint64_t m = 0ull - static_cast<uint64_t>(((f[i] != 0) & (kk < key_threshold) & ((kk & 3u) == 0u)));
    acc0 += (static_cast<uint64_t>(v[i]) & m);
  }

  uint64_t sum = acc0 + acc1 + acc2 + acc3;
  return sum;
}
