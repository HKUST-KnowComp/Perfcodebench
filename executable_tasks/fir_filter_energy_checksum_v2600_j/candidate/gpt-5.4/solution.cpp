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

      int64_t acc = 0;
      std::size_t j = 0;

      for (; j + 7 < lim; j += 8) {
        acc += static_cast<int64_t>(s[0]) * static_cast<int64_t>(t[0])
             + static_cast<int64_t>(s[-1]) * static_cast<int64_t>(t[1])
             + static_cast<int64_t>(s[-2]) * static_cast<int64_t>(t[2])
             + static_cast<int64_t>(s[-3]) * static_cast<int64_t>(t[3])
             + static_cast<int64_t>(s[-4]) * static_cast<int64_t>(t[4])
             + static_cast<int64_t>(s[-5]) * static_cast<int64_t>(t[5])
             + static_cast<int64_t>(s[-6]) * static_cast<int64_t>(t[6])
             + static_cast<int64_t>(s[-7]) * static_cast<int64_t>(t[7]);
        s -= 8;
        t += 8;
      }
      for (; j < lim; ++j) {
        acc += static_cast<int64_t>(*s--) * static_cast<int64_t>(*t++);
      }

      const uint64_t mix = static_cast<uint64_t>(acc * acc + 3 * acc);
      total ^= mix;
      total *= 1099511628211ULL;
    }
  }

  return total;
}
