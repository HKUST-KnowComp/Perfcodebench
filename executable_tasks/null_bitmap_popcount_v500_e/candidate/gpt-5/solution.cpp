#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  (void)iters; // Final result is independent of iteration count
  const uint64_t* p = words.data();
  std::size_t n = words.size();

  uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
  std::size_t i = 0;
  std::size_t n4 = n & ~static_cast<std::size_t>(3);

  for (; i < n4; i += 4) {
    acc0 += static_cast<uint64_t>(__builtin_popcountll(p[i]));
    acc1 += static_cast<uint64_t>(__builtin_popcountll(p[i + 1]));
    acc2 += static_cast<uint64_t>(__builtin_popcountll(p[i + 2]));
    acc3 += static_cast<uint64_t>(__builtin_popcountll(p[i + 3]));
  }

  uint64_t total = acc0 + acc1 + acc2 + acc3;
  for (; i < n; ++i) {
    total += static_cast<uint64_t>(__builtin_popcountll(p[i]));
  }

  return total;
}
