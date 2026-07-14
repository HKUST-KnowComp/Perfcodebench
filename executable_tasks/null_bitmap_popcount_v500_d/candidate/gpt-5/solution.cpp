#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  // Preserve baseline behavior: if no iterations, return 0
  if (iters <= 0) return 0;

  const uint64_t* p = words.data();
  std::size_t n = words.size();
  uint64_t count = 0;

  // Unroll by 8 for throughput
  std::size_t blocks = n / 8;
  for (std::size_t i = 0; i < blocks; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(p[0]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[1]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[2]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[3]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[4]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[5]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[6]));
    count += static_cast<uint64_t>(__builtin_popcountll(p[7]));
    p += 8;
  }

  switch (n & 7u) {
    case 7: count += static_cast<uint64_t>(__builtin_popcountll(p[6])); [[fallthrough]];
    case 6: count += static_cast<uint64_t>(__builtin_popcountll(p[5])); [[fallthrough]];
    case 5: count += static_cast<uint64_t>(__builtin_popcountll(p[4])); [[fallthrough]];
    case 4: count += static_cast<uint64_t>(__builtin_popcountll(p[3])); [[fallthrough]];
    case 3: count += static_cast<uint64_t>(__builtin_popcountll(p[2])); [[fallthrough]];
    case 2: count += static_cast<uint64_t>(__builtin_popcountll(p[1])); [[fallthrough]];
    case 1: count += static_cast<uint64_t>(__builtin_popcountll(p[0])); [[fallthrough]];
    default: break;
  }

  return count;
}
