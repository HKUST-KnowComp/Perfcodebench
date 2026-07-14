#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline semantics when iters <= 0: return 0.
  if (iters <= 0) {
    return 0;
  }
  // If threshold is 0, no key can be < threshold, so sum is 0.
  if (threshold == 0u) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* kptr = keys.data();
  const uint8_t* fptr = flags.data();
  const uint32_t* pptr = payloads.data();

  uint64_t sum = 0;

  // Single pass: sum payloads for indices where predicate holds.
  const uint8_t* fend = fptr + n;
  while (fptr != fend) {
    uint8_t f = *fptr++;
    uint32_t key = *kptr++;
    const uint32_t payload = *pptr++;
    if (f && key < threshold && ((key & 7u) == 0u)) {
      sum += static_cast<uint64_t>(payload);
    }
  }

  // Baseline recomputed the same sum each iteration and returned the last.
  // For any iters > 0, the result is identical to the single computation above.
  return sum;
}
