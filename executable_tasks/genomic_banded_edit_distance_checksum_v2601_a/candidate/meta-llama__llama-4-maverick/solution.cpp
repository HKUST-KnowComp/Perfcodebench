#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

int banded_distance(const std::string& a, const std::string& b, int max_band) {
  const std::size_t n = a.size();
  const std::size_t m = b.size();
  const std::size_t band_size = static_cast<std::size_t>(max_band) * 2 + 1;
  std::vector<int> dp(band_size * (n + 1u), INT_MAX);
  dp[0] = 0;
  for (std::size_t j = 1; j <= m; ++j) {
    const std::size_t start = (j > static_cast<std::size_t>(max_band)) ? (j - static_cast<std::size_t>(max_band)) : 0;
    const std::size_t end = std::min(n, j + static_cast<std::size_t>(max_band));
    for (std::size_t i = start; i <= end; ++i) {
      const std::size_t idx = i - (j - max_band);
      int best = INT_MAX;
      if (i > start) best = std::min(best, dp[(i - 1 - (j - max_band)) * band_size + idx - 1] + 1);  // del
      if (i < end) best = std::min(best, dp[(i + 1 - (j - max_band)) * band_size + idx + 1] + 1);  // ins
      best = std::min(best, dp[(i - (j - max_band)) * band_size + idx] + ((i > 0 && i <= n && j <= m) ? (a[i - 1] == b[j - 1] ? 0 : 1) : 1));  // sub or init
      dp[(i - (j - max_band)) * band_size + idx] = best;
    }
  }
  return dp[(m - (m - max_band)) * band_size + std::min(static_cast<std::size_t>(max_band) * 2, n - (m - max_band))];
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      const int d = banded_distance(lhs[i], rhs[i], max_band);
      hash ^= static_cast<uint64_t>(d + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
