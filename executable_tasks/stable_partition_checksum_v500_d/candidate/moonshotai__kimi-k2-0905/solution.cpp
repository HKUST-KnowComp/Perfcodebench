#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  static thread_local std::vector<uint32_t> out;
  if (out.size() < n) out.resize(n);

  uint32_t* __restrict dst = out.data();
  const uint32_t* __restrict src = values.data();
  const uint8_t* __restrict flg = flags.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t left_cnt = 0;
    for (std::size_t i = 0; i < n; ++i) {
      left_cnt += flg[i];
    }
    const std::size_t right_cnt = n - left_cnt;

    uint32_t* left_ptr  = dst;
    uint32_t* right_ptr = dst + left_cnt + right_cnt - 1;

    for (std::size_t i = 0; i < n; ++i) {
      if (flg[i]) {
        *left_ptr++ = src[i];
      } else {
        *right_ptr-- = src[i];
      }
    }

    hash = 1469598103934665603ULL;
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      hash = mix(hash, dst[i + 0]);
      hash = mix(hash, dst[i + 1]);
      hash = mix(hash, dst[i + 2]);
      hash = mix(hash, dst[i + 3]);
    }
    for (; i < n; ++i) {
      hash = mix(hash, dst[i]);
    }
  }
  return hash;
}
