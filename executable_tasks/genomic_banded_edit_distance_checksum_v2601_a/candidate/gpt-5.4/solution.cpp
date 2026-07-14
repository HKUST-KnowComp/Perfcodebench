#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline int min3(int a, int b, int c) {
  int m = a < b ? a : b;
  return c < m ? c : m;
}

// Exact Levenshtein distance restricted to a diagonal band when possible.
// Falls back to full two-row DP if the true distance may lie outside the band.
int edit_distance_banded_or_full(const std::string& a,
                                 const std::string& b,
                                 int max_band,
                                 std::vector<int>& prev,
                                 std::vector<int>& curr) {
  const int n = static_cast<int>(a.size());
  const int m = static_cast<int>(b.size());

  if (n == 0) return m;
  if (m == 0) return n;

  const int len_diff = n > m ? (n - m) : (m - n);

  // If the length difference already exceeds the band, an exact banded DP with
  // this width cannot contain the optimal path. Use full DP.
  const bool use_band = max_band >= 0 && len_diff <= max_band;

  if (!use_band) {
    if (static_cast<int>(prev.size()) < m + 1) {
      prev.resize(static_cast<std::size_t>(m + 1));
      curr.resize(static_cast<std::size_t>(m + 1));
    }

    for (int j = 0; j <= m; ++j) prev[static_cast<std::size_t>(j)] = j;

    for (int i = 1; i <= n; ++i) {
      curr[0] = i;
      const char ca = a[static_cast<std::size_t>(i - 1)];
      for (int j = 1; j <= m; ++j) {
        const int cost = (ca == b[static_cast<std::size_t>(j - 1)]) ? 0 : 1;
        curr[static_cast<std::size_t>(j)] = min3(
            prev[static_cast<std::size_t>(j)] + 1,
            curr[static_cast<std::size_t>(j - 1)] + 1,
            prev[static_cast<std::size_t>(j - 1)] + cost);
      }
      prev.swap(curr);
    }
    return prev[static_cast<std::size_t>(m)];
  }

  const int INF = n + m + 1;
  if (static_cast<int>(prev.size()) < m + 1) {
    prev.resize(static_cast<std::size_t>(m + 1));
    curr.resize(static_cast<std::size_t>(m + 1));
  }

  for (int j = 0; j <= m; ++j) prev[static_cast<std::size_t>(j)] = INF;
  const int init_hi = m < max_band ? m : max_band;
  for (int j = 0; j <= init_hi; ++j) prev[static_cast<std::size_t>(j)] = j;

  for (int i = 1; i <= n; ++i) {
    const int lo = (i - max_band) > 1 ? (i - max_band) : 1;
    const int hi = (i + max_band) < m ? (i + max_band) : m;

    if (lo > hi) {
      return len_diff;
    }

    curr[static_cast<std::size_t>(lo - 1)] = (lo == 1) ? i : INF;
    const char ca = a[static_cast<std::size_t>(i - 1)];

    for (int j = lo; j <= hi; ++j) {
      const int cost = (ca == b[static_cast<std::size_t>(j - 1)]) ? 0 : 1;
      curr[static_cast<std::size_t>(j)] = min3(
          prev[static_cast<std::size_t>(j)] + 1,
          curr[static_cast<std::size_t>(j - 1)] + 1,
          prev[static_cast<std::size_t>(j - 1)] + cost);
    }

    if (hi < m) curr[static_cast<std::size_t>(hi + 1)] = INF;
    prev.swap(curr);
  }

  return prev[static_cast<std::size_t>(m)];
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  const std::size_t count = lhs.size();
  if (count == 0 || iters <= 0) {
    return 0;
  }

  std::vector<int> distances(count);
  std::vector<int> prev;
  std::vector<int> curr;

  // Distances are invariant across iterations; compute once and only repeat the
  // checksum accumulation.
  for (std::size_t i = 0; i < count; ++i) {
    distances[i] = edit_distance_banded_or_full(lhs[i], rhs[i], max_band, prev, curr);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < count; ++i) {
      hash ^= static_cast<uint64_t>(distances[i] + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
