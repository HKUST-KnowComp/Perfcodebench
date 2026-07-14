#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  uint64_t sum = 0;
  const size_t P = probes.size();
  const size_t I = starts.size();
  const uint32_t* s = starts.data();
  const uint32_t* p = payloads.data();
  const uint32_t* w = weights.data();
  const uint32_t* pr = probes.data();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t idx = 0;
    for (size_t i = 0; i < P; ++i) {
      uint32_t probe = pr[i];
      while (idx + 1 < I && s[idx + 1] <= probe) {
        ++idx;
      }
      sum += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[i]);
    }
  }
  return sum;
}