#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t N = signal.size();
  const std::size_t T = taps.size();

  const int16_t* s = N ? signal.data() : nullptr;
  const int16_t* h = T ? taps.data() : nullptr;

  uint64_t total = 1469598103934665603ULL; // FNV offset basis
  const uint64_t FNV_prime = 1099511628211ULL;

  for (std::size_t i = 0; i < N; ++i) {
    int64_t acc = 0;
    std::size_t maxj = T;
    if (i + 1 < maxj) maxj = i + 1;

    const int16_t* ps = s + i; // points at signal[i]
    const int16_t* ph = h;     // points at taps[0]

    std::size_t j = maxj;
    // Unroll by 4
    while (j >= 4) {
      acc += static_cast<int64_t>(ps[0]) * static_cast<int64_t>(ph[0]);
      acc += static_cast<int64_t>(ps[-1]) * static_cast<int64_t>(ph[1]);
      acc += static_cast<int64_t>(ps[-2]) * static_cast<int64_t>(ph[2]);
      acc += static_cast<int64_t>(ps[-3]) * static_cast<int64_t>(ph[3]);
      ps -= 4;
      ph += 4;
      j -= 4;
    }
    while (j > 0) {
      acc += static_cast<int64_t>(*ps--) * static_cast<int64_t>(*ph++);
      --j;
    }

    // Preserve signed 64-bit overflow behavior as in baseline
    int64_t energy_term = acc * acc + 3 * acc;
    total ^= static_cast<uint64_t>(energy_term);
    total *= FNV_prime;
  }

  return total;
}
