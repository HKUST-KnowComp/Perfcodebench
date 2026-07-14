#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr int kInf = 1 << 20;

inline int min3(int a, int b, int c) {
  int x = a < b ? a : b;
  return x < c ? x : c;
}

int banded_distance(const std::string& a, const std::string& b, int band) {
  const int n = static_cast<int>(a.size());
  const int m = static_cast<int>(b.size());
  const int width = 2 * band + 1;
  std::vector<int> prev(static_cast<std::size_t>(width), kInf);
  std::vector<int> cur(static_cast<std::size_t>(width), kInf);

  for (int j = 0; j <= band && j <= m; ++j) {
    prev[static_cast<std::size_t>(j + band)] = j;
  }

  for (int i = 1; i <= n; ++i) {
    std::fill(cur.begin(), cur.end(), kInf);
    const int j_lo = i - band > 0 ? i - band : 0;
    const int j_hi = i + band < m ? i + band : m;
    for (int j = j_lo; j <= j_hi; ++j) {
      const int idx = j - i + band;
      int best = kInf;
      if (j <= i + band - 1 && idx + 1 < width) {
        const int del = prev[static_cast<std::size_t>(idx + 1)] + 1;
        if (del < best) best = del;
      }
      if (j >= i - band + 1 && idx - 1 >= 0) {
        const int ins = cur[static_cast<std::size_t>(idx - 1)] + 1;
        if (ins < best) best = ins;
      }
      if (idx >= 0 && idx < width) {
        const int sub = prev[static_cast<std::size_t>(idx)] + ((j == 0 || a[static_cast<std::size_t>(i - 1)] == b[static_cast<std::size_t>(j - 1)]) ? 0 : 1);
        best = min3(best, sub, kInf);
      }
      if (j == 0) best = i;
      cur[static_cast<std::size_t>(idx)] = best;
    }
    prev.swap(cur);
  }
  return prev[static_cast<std::size_t>(m - n + band)];
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
