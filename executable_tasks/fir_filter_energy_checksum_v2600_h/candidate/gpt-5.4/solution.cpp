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

  const int16_t* const s = signal.data();
  const int16_t* const t = taps.data();

  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t lim = (i + 1 < m) ? (i + 1) : m;
      int64_t acc = 0;

      std::size_t j = 0;
      for (; j + 7 < lim; j += 8) {
        acc += static_cast<int64_t>(s[i - j]) * static_cast<int64_t>(t[j]);
        acc += static_cast<int64_t>(s[i - (j + 1)]) * static_cast<int64_t>(t[j + 1]);
        acc += static_cast<int64_t>(s[i - (j + 2)]) * static_cast<int64_t>(t[j + 2]);
        acc += static_cast<int64_t>(s[i - (j + 3)]) * static_cast<int64_t>(t[j + 3]);
        acc += static_cast<int64_t>(s[i - (j + 4)]) * static_cast<int64_t>(t[j + 4]);
        acc += static_cast<int64_t>(s[i - (j + 5)]) * static_cast<int64_t>(t[j + 5]);
        acc += static_cast<int64_t>(s[i - (j + 6)]) * static_cast<int64_t>(t[j + 6]);
        acc += static_cast<int64_t>(s[i - (j + 7)]) * static_cast<int64_t>(t[j + 7]);
      }
      for (; j < lim; ++j) {
        acc += static_cast<int64_t>(s[i - j]) * static_cast<int64_t>(t[j]);
      }

      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }

  return total;
}
