#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0; otherwise, the result
  // equals the checksum of a single full pass (baseline overwrote total each iter).
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t N = signal.size();
  const std::size_t M = taps.size();

  // FNV-1a 64-bit parameters as used in baseline
  uint64_t total = 1469598103934665603ULL; // offset basis

  // If signal is empty, baseline sets total to offset basis and returns it.
  if (N == 0) {
    return total;
  }

  const int16_t* s = signal.data();
  const int16_t* t = taps.data();

  for (std::size_t i = 0; i < N; ++i) {
    // j ranges from 0 to min(i, M-1)
    std::size_t jmax = M;
    const std::size_t lim = i + 1;
    if (jmax > lim) jmax = lim;

    int64_t acc = 0;
    const int16_t* sp = s + i; // points to signal[i]

    // Unroll inner loop by 4
    std::size_t j = 0;
    std::size_t blocks = jmax / 4;
    for (std::size_t b = 0; b < blocks; ++b) {
      // Accumulate 4 products
      acc += static_cast<int64_t>(sp[0])  * static_cast<int64_t>(t[j + 0]);
      acc += static_cast<int64_t>(sp[-1]) * static_cast<int64_t>(t[j + 1]);
      acc += static_cast<int64_t>(sp[-2]) * static_cast<int64_t>(t[j + 2]);
      acc += static_cast<int64_t>(sp[-3]) * static_cast<int64_t>(t[j + 3]);
      sp -= 4;
      j += 4;
    }
    for (; j < jmax; ++j) {
      acc += static_cast<int64_t>(*sp) * static_cast<int64_t>(t[j]);
      --sp;
    }

    // Hash exactly as in baseline: static_cast<uint64_t>(acc * acc + 3 * acc)
    int64_t expr = acc * acc + 3 * acc;
    total ^= static_cast<uint64_t>(expr);
    total *= 1099511628211ULL; // FNV prime
  }

  return total;
}
