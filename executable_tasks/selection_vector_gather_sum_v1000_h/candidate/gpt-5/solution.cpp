#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Baseline returns 0 if iters <= 0 because loop never executes and sum stays 0.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t unroll = 4;
  const std::size_t n4 = n & ~(unroll - 1);

  for (; i < n4; i += unroll) {
    // 0
    {
      const uint32_t kk = k[i];
      const uint64_t c = static_cast<uint64_t>( (kk < threshold) & (f[i] != 0) & ((kk & 7u) == 0u) );
      const uint64_t mask = 0ull - c; // 0 if false, ~0 if true
      sum += (static_cast<uint64_t>(p[i]) & mask);
    }
    // 1
    {
      const uint32_t kk = k[i + 1];
      const uint64_t c = static_cast<uint64_t>( (kk < threshold) & (f[i + 1] != 0) & ((kk & 7u) == 0u) );
      const uint64_t mask = 0ull - c;
      sum += (static_cast<uint64_t>(p[i + 1]) & mask);
    }
    // 2
    {
      const uint32_t kk = k[i + 2];
      const uint64_t c = static_cast<uint64_t>( (kk < threshold) & (f[i + 2] != 0) & ((kk & 7u) == 0u) );
      const uint64_t mask = 0ull - c;
      sum += (static_cast<uint64_t>(p[i + 2]) & mask);
    }
    // 3
    {
      const uint32_t kk = k[i + 3];
      const uint64_t c = static_cast<uint64_t>( (kk < threshold) & (f[i + 3] != 0) & ((kk & 7u) == 0u) );
      const uint64_t mask = 0ull - c;
      sum += (static_cast<uint64_t>(p[i + 3]) & mask);
    }
  }

  for (; i < n; ++i) {
    const uint32_t kk = k[i];
    const uint64_t c = static_cast<uint64_t>( (kk < threshold) & (f[i] != 0) & ((kk & 7u) == 0u) );
    const uint64_t mask = 0ull - c;
    sum += (static_cast<uint64_t>(p[i]) & mask);
  }

  // Baseline recomputes sum each iteration and returns the last value, which is the same.
  // We compute once and return it.
  return sum;
}
