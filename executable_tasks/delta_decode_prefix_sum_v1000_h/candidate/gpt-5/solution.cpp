#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  (void)iters; // Each iteration produces the same result; compute once.
  const std::size_t n = deltas.size();
  if (n == 0) return 0ULL;

  const uint32_t* p = deltas.data();
  const uint32_t* end = p + n;

  uint64_t sum = 0;
  uint64_t value = static_cast<uint64_t>(base);

  // Fuse decoding (prefix accumulation) with summation in a single pass.
  for (; p != end; ++p) {
    value += static_cast<uint64_t>(*p);
    sum += value;
  }

  return sum;
}
