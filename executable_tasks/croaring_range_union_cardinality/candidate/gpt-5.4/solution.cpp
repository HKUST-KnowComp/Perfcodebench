#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  const std::size_t n = starts.size();
  if (n == 0) return 0;

  struct Interval {
    uint32_t start;
    uint32_t end;
  };

  std::vector<Interval> intervals;
  intervals.reserve(n);

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t len = lengths[i];
    if (len == 0) continue;

    const uint32_t start = starts[i];
    const uint64_t end64 = static_cast<uint64_t>(start) + static_cast<uint64_t>(len);
    const uint32_t end = end64 >= (static_cast<uint64_t>(UINT32_MAX) + 1ULL)
                             ? 0u
                             : static_cast<uint32_t>(end64);

    if (end == 0u) {
      return uint64_t{1} << 32;
    }

    intervals.push_back({start, end});
  }

  if (intervals.empty()) return 0;

  std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
    return a.start < b.start;
  });

  uint64_t total = 0;
  uint32_t cur_start = intervals[0].start;
  uint32_t cur_end = intervals[0].end;

  for (std::size_t i = 1; i < intervals.size(); ++i) {
    const uint32_t s = intervals[i].start;
    const uint32_t e = intervals[i].end;

    if (s <= cur_end) {
      if (e > cur_end) cur_end = e;
    } else {
      total += static_cast<uint64_t>(cur_end) - static_cast<uint64_t>(cur_start);
      cur_start = s;
      cur_end = e;
    }
  }

  total += static_cast<uint64_t>(cur_end) - static_cast<uint64_t>(cur_start);
  return total;
}
