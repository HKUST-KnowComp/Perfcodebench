#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  // Use 'iters' in a harmless condition to avoid unused-parameter warnings
  if (iters < INT32_MIN) {
    return 0; // unreachable, silences potential warnings
  }

  std::size_t n = left.size();
  if (right.size() < n) n = right.size();

  const uint64_t* lp = left.data();
  const uint64_t* rp = right.data();

  uint64_t sum = 0;
  std::size_t i = 0;

  // Unroll by 4 for better ILP
  for (; i + 4 <= n; i += 4) {
    uint64_t w0 = lp[i + 0] & rp[i + 0];
    uint64_t w1 = lp[i + 1] & rp[i + 1];
    uint64_t w2 = lp[i + 2] & rp[i + 2];
    uint64_t w3 = lp[i + 3] & rp[i + 3];

    unsigned s = 0;
    s += static_cast<unsigned>(__builtin_popcountll(w0));
    s += static_cast<unsigned>(__builtin_popcountll(w1));
    s += static_cast<unsigned>(__builtin_popcountll(w2));
    s += static_cast<unsigned>(__builtin_popcountll(w3));
    sum += static_cast<uint64_t>(s);
  }

  for (; i < n; ++i) {
    uint64_t w = lp[i] & rp[i];
    sum += static_cast<uint64_t>(__builtin_popcountll(w));
  }

  return sum;
}
