#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

int full_distance(const std::string& a, const std::string& b) {
  const std::size_t n = a.size();
  const std::size_t m = b.size();
  std::vector<int> dp((n + 1u) * (m + 1u), 0);
  for (std::size_t i = 0; i <= n; ++i) dp[i * (m + 1u)] = static_cast<int>(i);
  for (std::size_t j = 0; j <= m; ++j) dp[j] = static_cast<int>(j);
  for (std::size_t i = 1; i <= n; ++i) {
    for (std::size_t j = 1; j <= m; ++j) {
      const int cost = (a[i - 1u] == b[j - 1u]) ? 0 : 1;
      const int del = dp[(i - 1u) * (m + 1u) + j] + 1;
      const int ins = dp[i * (m + 1u) + (j - 1u)] + 1;
      const int sub = dp[(i - 1u) * (m + 1u) + (j - 1u)] + cost;
      int best = del < ins ? del : ins;
      if (sub < best) best = sub;
      dp[i * (m + 1u) + j] = best;
    }
  }
  return dp[n * (m + 1u) + m];
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  (void)max_band;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      const int d = full_distance(lhs[i], rhs[i]);
      hash ^= static_cast<uint64_t>(d + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
