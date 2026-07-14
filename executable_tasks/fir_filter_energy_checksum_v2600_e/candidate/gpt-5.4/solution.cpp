#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const std::size_t n = signal.size();
  const std::size_t m = taps.size();

  if (iters <= 0) {
    return 0;
  }

  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t lim = (i + 1 < m) ? (i + 1) : m;
      const int16_t* s = signal.data() + i;
      const int16_t* t = taps.data();

      int64_t acc0 = 0;
      int64_t acc1 = 0;
      int64_t acc2 = 0;
      int64_t acc3 = 0;

      std::size_t j = 0;
      for (; j + 3 < lim; j += 4) {
        acc0 += static_cast<int64_t>(s[0]) * static_cast<int64_t>(t[0]);
        acc1 += static_cast<int64_t>(s[-1]) * static_cast<int64_t>(t[1]);
        acc2 += static_cast<int64_t>(s[-2]) * static_cast<int64_t>(t[2]);
        acc3 += static_cast<int64_t>(s[-3]) * static_cast<int64_t>(t[3]);
        s -= 4;
        t += 4;
      }

      int64_t acc = acc0 + acc1 + acc2 + acc3;
      for (; j < lim; ++j) {
        acc += static_cast<int64_t>(*s--) * static_cast<int64_t>(*t++);
      }

      const int64_t e = acc * acc + 3 * acc;
      total ^= static_cast<uint64_t>(e);
      total *= 1099511628211ULL;
    }
  }

  return total;
}
