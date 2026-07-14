#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {

  const std::size_t n = lhs_starts.size();
  const std::size_t m = rhs_starts.size();

  // Build and sort interval lists (start, end) by start position
  std::vector<std::pair<uint32_t, uint32_t>> lhs(n), rhs(m);
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t s = lhs_starts[i];
    lhs[i] = {s, s + lhs_lengths[i]};
  }
  for (std::size_t i = 0; i < m; ++i) {
    uint32_t s = rhs_starts[i];
    rhs[i] = {s, s + rhs_lengths[i]};
  }
  auto cmp = [](const auto& a, const auto& b) { return a.first < b.first; };
  std::sort(lhs.begin(), lhs.end(), cmp);
  std::sort(rhs.begin(), rhs.end(), cmp);

  // Merge overlapping/adjacent intervals in each side to simplify scan
  auto coalesce = [](std::vector<std::pair<uint32_t, uint32_t>>& v) {
    if (v.empty()) return;
    std::size_t w = 0;
    for (std::size_t r = 1; r < v.size(); ++r) {
      if (v[r].first <= v[w].second) {
        v[w].second = std::max(v[w].second, v[r].second);
      } else {
        ++w;
        v[w] = v[r];
      }
    }
    v.resize(w + 1);
  };
  coalesce(lhs);
  coalesce(rhs);

  // Two-finger sweep to compute symmetric difference cardinality
  uint64_t count = 0;
  std::size_t i = 0, j = 0;
  while (i < lhs.size() && j < rhs.size()) {
    auto& L = lhs[i];
    auto& R = rhs[j];
    if (L.second <= R.first) {
      count += L.second - L.first; // L entirely before R
      ++i;
    } else if (R.second <= L.first) {
      count += R.second - R.first; // R entirely before L
      ++j;
    } else {
      // Intersection or overlap
      if (L.first < R.first) {
        count += R.first - L.first; // left stub of L
        L.first = R.first;
      } else if (R.first < L.first) {
        count += L.first - R.first; // left stub of R
        R.first = L.first;
      }
      // Now L.first == R.first
      uint32_t common_end = std::min(L.second, R.second);
      // symmetric difference inside [L.first, common_end)
      // nothing added here; values are in both sets
      if (L.second == common_end) {
        ++i;
      } else {
        L.first = common_end;
      }
      if (R.second == common_end) {
        ++j;
      } else {
        R.first = common_end;
      }
    }
  }
  // Tails
  while (i < lhs.size()) {
    count += lhs[i].second - lhs[i].first;
    ++i;
  }
  while (j < rhs.size()) {
    count += rhs[j].second - rhs[j].first;
    ++j;
  }
  return count;
}