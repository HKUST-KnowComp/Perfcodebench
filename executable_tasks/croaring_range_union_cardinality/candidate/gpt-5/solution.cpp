#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  const std::size_t n = starts.size();
  if (n == 0) return 0;

  // Build list of half-open intervals [start, end), clamped to [0, 2^32]
  // and skipping empty ones.
  struct Interval { uint64_t s, e; };
  std::vector<Interval> intervals;
  intervals.reserve(n);

  const uint64_t DOMAIN_LIMIT = 4294967296ULL; // 2^32, exclusive upper bound

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t len = lengths[i];
    if (len == 0) continue;
    uint64_t s = static_cast<uint64_t>(starts[i]);
    uint64_t e = s + static_cast<uint64_t>(len); // compute in 64-bit to avoid wrap
    if (e > DOMAIN_LIMIT) e = DOMAIN_LIMIT;      // clamp to domain limit
    if (e <= s) continue;                        // still empty after clamping
    intervals.push_back({s, e});
  }

  if (intervals.empty()) return 0;

  std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
    if (a.s < b.s) return true;
    if (a.s > b.s) return false;
    return a.e < b.e;
  });

  // Merge intervals and accumulate total covered length.
  uint64_t total = 0;
  uint64_t cur_s = intervals[0].s;
  uint64_t cur_e = intervals[0].e;

  for (std::size_t i = 1; i < intervals.size(); ++i) {
    const uint64_t s = intervals[i].s;
    const uint64_t e = intervals[i].e;
    if (s > cur_e) {
      total += (cur_e - cur_s);
      cur_s = s;
      cur_e = e;
    } else if (e > cur_e) {
      cur_e = e;
    }
  }
  total += (cur_e - cur_s);

  return total;
}
