#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

namespace {

int banded_distance(const std::string& a, const std::string& b, int max_band, std::vector<int>& dp) {
  const std::size_t n = a.size();
  const std::size_t m = b.size();
  
  if (n < m) return banded_distance(b, a, max_band, dp);
  if (n - m > static_cast<std::size_t>(max_band)) return static_cast<int>(n - m);
  
  const int band_width = 2 * max_band + 1;
  if (dp.size() < static_cast<std::size_t>(band_width * 2)) {
    dp.resize(band_width * 2);
  }

  const size_t prev_row = 0;
  for (int j = 0; j <= std::min(static_cast<int>(m), max_band); ++j) {
    dp[prev_row * band_width + j] = j;
  }

  for (std::size_t i = 1; i <= n; ++i) {
    const size_t curr_row = i % 2;
    const size_t last_row = (i-1) % 2;
    const int start_j = std::max(0, static_cast<int>(i) - max_band);
    const int end_j = std::min(static_cast<int>(m), static_cast<int>(i) + max_band);

    std::fill(dp.begin() + curr_row * band_width, dp.begin() + curr_row * band_width + band_width, std::numeric_limits<int>::max() / 2);

    for (int j = start_j; j <= end_j; ++j) {
      const int cost = (a[i-1] == b[j-1]) ? 0 : 1;
      const int del = dp[last_row * band_width + j] + 1;
      const int ins = dp[curr_row * band_width + (j-1)] + 1;
      const int sub = dp[last_row * band_width + (j-1)] + cost;
      dp[curr_row * band_width + j] = std::min(std::min(del, ins), sub);
    }
  }
  return dp[(n%2) * band_width + m];
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  uint64_t hash = 0;
  std::vector<int> shared_dp;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      const int d = banded_distance(lhs[i], rhs[i], max_band, shared_dp);
      hash ^= static_cast<uint64_t>(d + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}