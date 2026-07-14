#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0; otherwise, result equals a single iteration
  if (iters <= 0) return 0ULL;

  const std::size_t n = signal.size();
  const std::size_t m = taps.size();
  const int16_t* s = signal.data();
  const int16_t* t = taps.data();

  uint64_t total = 1469598103934665603ULL; // FNV offset basis

  for (std::size_t i = 0; i < n; ++i) {
    int64_t acc = 0;
    std::size_t len = m;
    const std::size_t im1 = i + 1;
    if (len > im1) len = im1;

    const int16_t* sp = s + i;      // points to signal[i]
    const int16_t* tp = t;          // points to taps[0]
    std::size_t k = len;

    // Unroll by 4 for better throughput
    while (k >= 4) {
      acc += static_cast<int64_t>(sp[0]) * static_cast<int64_t>(tp[0]);
      acc += static_cast<int64_t>(sp[-1]) * static_cast<int64_t>(tp[1]);
      acc += static_cast<int64_t>(sp[-2]) * static_cast<int64_t>(tp[2]);
      acc += static_cast<int64_t>(sp[-3]) * static_cast<int64_t>(tp[3]);
      sp -= 4;
      tp += 4;
      k -= 4;
    }
    while (k) {
      acc += static_cast<int64_t>(sp[0]) * static_cast<int64_t>(tp[0]);
      --sp;
      ++tp;
      --k;
    }

    // Preserve exact operation ordering/types as baseline
    uint64_t mix = static_cast<uint64_t>(acc * acc + 3 * acc);
    total ^= mix;
    total *= 1099511628211ULL; // FNV prime
  }

  return total;
}
