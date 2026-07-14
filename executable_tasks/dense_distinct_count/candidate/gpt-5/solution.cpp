#include "interface.h"

#include <unordered_set>
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)iters;
  const size_t n = keys.size();
  if (n == 0) return 0;

  // Heuristic: use dense marking when key_space is reasonably small compared to n
  const uint64_t threshold = static_cast<uint64_t>(n) * 8ull;
  if (key_space > 0 && static_cast<uint64_t>(key_space) <= threshold) {
    std::vector<uint32_t> marks(key_space, 0);
    uint32_t* mark_data = marks.data();
    uint64_t cnt = 0;

    // Handle out-of-range keys via a separate set (disjoint from in-range)
    std::unordered_set<uint32_t> overflow;
    bool have_overflow = false;

    for (uint32_t k : keys) {
      if (k < key_space) {
        uint32_t &m = mark_data[k];
        if (m == 0) {
          m = 1;
          ++cnt;
        }
      } else {
        if (!have_overflow) {
          have_overflow = true;
          overflow.reserve(4);
        }
        overflow.insert(k);
      }
    }
    if (have_overflow) cnt += static_cast<uint64_t>(overflow.size());
    return cnt;
  } else {
    // Fallback for large key_space: use a hash set over all keys
    std::unordered_set<uint32_t> seen;
    seen.reserve(n);
    for (uint32_t k : keys) {
      seen.insert(k);
    }
    return static_cast<uint64_t>(seen.size());
  }
}
