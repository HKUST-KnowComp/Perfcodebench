#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = left.size();
  const std::size_t m = right.size();
  if (n == 0 || m == 0) return 0;

  const std::size_t limit = n < m ? n : m;
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t count = 0;
  std::size_t i = 0;

  for (; i + 8 <= limit; i += 8) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 4] & r[i + 4]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 5] & r[i + 5]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 6] & r[i + 6]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 7] & r[i + 7]));
  }
  for (; i + 4 <= limit; i += 4) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
  }
  for (; i + 2 <= limit; i += 2) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    count += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
  }
  for (; i < limit; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }

  return count;
}
