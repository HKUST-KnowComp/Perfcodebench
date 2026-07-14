#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint8_t* __restrict f = flags.data();
  const uint32_t* __restrict p = payloads.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    
    #pragma GCC ivdep
    #pragma clang loop vectorize(enable) interleave(enable)
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = k[i];
      uint32_t payload = p[i];
      uint8_t flag = f[i];
      
      bool valid = flag && (key < threshold) && ((key & 7u) == 0u);
      current_sum += valid ? payload : 0;
    }
    
    sum = current_sum;
    asm volatile("" : "+g"(sum));
  }
  return sum;
}
