#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

static inline std::vector<std::pair<uint64_t, uint64_t>> build_union(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  const uint64_t MAXP1 = static_cast<uint64_t>(UINT32_MAX) + 1ULL;

  std::vector<std::pair<uint64_t, uint64_t>> intervals;
  intervals.reserve(starts.size());

  const size_t n = starts.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t s32 = starts[i];
    uint32_t len32 = lengths[i];
    if (len32 == 0) continue; // empty interval
    uint64_t s = static_cast<uint64_t>(s32);
    uint64_t e = s + static_cast<uint64_t>(len32);
    if (e > MAXP1) e = MAXP1; // cap at 2^32 (exclusive upper bound)
    if (e <= s) continue; // in case of overflow anomalies, skip
    intervals.emplace_back(s, e);
  }

  if (intervals.empty()) return intervals;

  std::sort(intervals.begin(), intervals.end(), [](const auto& a, const auto& b) {
    if (a.first != b.first) return a.first < b.first;
    return a.second < b.second;
  });

  // Merge overlapping or contiguous intervals
  std::vector<std::pair<uint64_t, uint64_t>> merged;
  merged.reserve(intervals.size());
  uint64_t cur_s = intervals[0].first;
  uint64_t cur_e = intervals[0].second;
  for (size_t i = 1; i < intervals.size(); ++i) {
    uint64_t s = intervals[i].first;
    uint64_t e = intervals[i].second;
    if (s <= cur_e) { // overlap or contiguous (since half-open, s==cur_e joins)
      if (e > cur_e) cur_e = e;
    } else {
      merged.emplace_back(cur_s, cur_e);
      cur_s = s;
      cur_e = e;
    }
  }
  merged.emplace_back(cur_s, cur_e);

  return merged;
}

} // namespace

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  // Build union of intervals for each side
  std::vector<std::pair<uint64_t, uint64_t>> L = build_union(lhs_starts, lhs_lengths);
  std::vector<std::pair<uint64_t, uint64_t>> R = build_union(rhs_starts, rhs_lengths);

  uint64_t total = 0;
  size_t i = 0, j = 0;
  while (i < L.size() && j < R.size()) {
    const auto& a = L[i];
    const auto& b = R[j];

    uint64_t s = (a.first > b.first) ? a.first : b.first;
    uint64_t e = (a.second < b.second) ? a.second : b.second;
    if (e > s) {
      total += (e - s);
    }

    if (a.second <= b.second) {
      ++i;
    } else {
      ++j;
    }
  }

  return total;
}
