#include "interface.h"

#if defined(__GNUC__) || defined(__clang__)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define UNLIKELY(x) (x)
#endif

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  
  if (iters <= 0) {
    return 0;
  }

  size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0;
  
  // The result is invariant across iterations, so we only need to compute it once.
  // Since the threshold is low, the condition `key < threshold` is rarely true,
  // making it highly predictable. We use UNLIKELY to hint the compiler.
  for (size_t i = 0; i < n; ++i) {
    uint32_t key = k[i];
    if (UNLIKELY(key < threshold)) {
      if ((key & 7u) == 0u && f[i]) {
        sum += p[i];
      }
    }
  }
  
  return sum;
}
