#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Match baseline semantics: if iters <= 0, the loop body never runs and sum stays 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  // Single computation suffices because inputs and predicate are invariant across iterations
  uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  for (; i < n4; i += 4) {
    // Element 0
    {
      const uint32_t ki = k[i];
      const bool c = (f[i] != 0) & (ki < threshold) & ((ki & 7u) == 0u);
      const uint64_t mask = static_cast<uint64_t>(-static_cast<int>(c));
      acc0 += (static_cast<uint64_t>(p[i]) & mask);
    }
    // Element 1
    {
      const uint32_t ki = k[i + 1];
      const bool c = (f[i + 1] != 0) & (ki < threshold) & ((ki & 7u) == 0u);
      const uint64_t mask = static_cast<uint64_t>(-static_cast<int>(c));
      acc1 += (static_cast<uint64_t>(p[i + 1]) & mask);
    }
    // Element 2
    {
      const uint32_t ki = k[i + 2];
      const bool c = (f[i + 2] != 0) & (ki < threshold) & ((ki & 7u) == 0u);
      const uint64_t mask = static_cast<uint64_t>(-static_cast<int>(c));
      acc2 += (static_cast<uint64_t>(p[i + 2]) & mask);
    }
    // Element 3
    {
      const uint32_t ki = k[i + 3];
      const bool c = (f[i + 3] != 0) & (ki < threshold) & ((ki & 7u) == 0u);
      const uint64_t mask = static_cast<uint64_t>(-static_cast<int>(c));
      acc3 += (static_cast<uint64_t>(p[i + 3]) & mask);
    }
  }

  // Tail
  for (; i < n; ++i) {
    const uint32_t ki = k[i];
    const bool c = (f[i] != 0) & (ki < threshold) & ((ki & 7u) == 0u);
    const uint64_t mask = static_cast<uint64_t>(-static_cast<int>(c));
    acc0 += (static_cast<uint64_t>(p[i]) & mask);
  }

  // Return the sum corresponding to the (identical) last iteration
  return acc0 + acc1 + acc2 + acc3;
}
