#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const std::size_t n = signal.size();
  const std::size_t m = taps.size();

  uint64_t total = 0;
  if (iters <= 0) return total;

  if (n == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      total = 1469598103934665603ULL;
    }
    return total;
  }

  const int16_t* const s = signal.data();
  const int16_t* const t = taps.data();

  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; ++i) {
      int64_t acc = 0;

      const std::size_t lim = (i + 1 < m) ? (i + 1) : m;
      const int16_t* sp = s + i;
      const int16_t* tp = t;

      std::size_t j = 0;
      for (; j + 7 < lim; j += 8) {
        acc += static_cast<int64_t>(sp[0]) * static_cast<int64_t>(tp[0])
             + static_cast<int64_t>(sp[-1]) * static_cast<int64_t>(tp[1])
             + static_cast<int64_t>(sp[-2]) * static_cast<int64_t>(tp[2])
             + static_cast<int64_t>(sp[-3]) * static_cast<int64_t>(tp[3])
             + static_cast<int64_t>(sp[-4]) * static_cast<int64_t>(tp[4])
             + static_cast<int64_t>(sp[-5]) * static_cast<int64_t>(tp[5])
             + static_cast<int64_t>(sp[-6]) * static_cast<int64_t>(tp[6])
             + static_cast<int64_t>(sp[-7]) * static_cast<int64_t>(tp[7]);
        sp -= 8;
        tp += 8;
      }
      for (; j < lim; ++j) {
        acc += static_cast<int64_t>(*sp--) * static_cast<int64_t>(*tp++);
      }

      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }

  return total;
}
