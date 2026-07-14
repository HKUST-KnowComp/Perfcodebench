#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_u64(const std::vector<uint64_t>& values) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = values.size();
  const uint64_t* p = values.data();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= p[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  std::vector<uint64_t> out(n, 0);

  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 0;

  if (n == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_u64(out);
    }
    return hash;
  }

  if (window <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      for (std::size_t i = 0; i < n; ++i) {
        out[i] = 0;
      }
      hash = checksum_u64(out);
    }
    return hash;
  }

  const std::size_t w = static_cast<std::size_t>(window);
  const uint32_t* in = values.data();
  uint64_t* outp = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t running = 0;

    if (w >= n) {
      for (std::size_t i = 0; i < n; ++i) {
        running += static_cast<uint64_t>(in[i]);
        outp[i] = running;
      }
    } else {
      std::size_t i = 0;
      for (; i < w; ++i) {
        running += static_cast<uint64_t>(in[i]);
        outp[i] = running;
      }
      for (; i < n; ++i) {
        running += static_cast<uint64_t>(in[i]);
        running -= static_cast<uint64_t>(in[i - w]);
        outp[i] = running;
      }
    }

    hash = checksum_u64(out);
  }

  return hash;
}
