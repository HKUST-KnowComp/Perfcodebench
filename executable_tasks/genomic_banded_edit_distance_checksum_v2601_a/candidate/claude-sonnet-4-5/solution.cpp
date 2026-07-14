#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

namespace {

int banded_distance(const std::string& a, const std::string& b, int max_band, std::vector<int>& prev, std::vector<int>& curr) {
  const int n = static_cast<int>(a.size());
  const int m = static_cast<int>(b.size());
  
  if (n == 0) return m;
  if (m == 0) return n;
  
  const int band = std::min(max_band, std::max(n, m));
  const int width = 2 * band + 1;
  
  prev.resize(width);
  curr.resize(width);
  
  const int INF = 1000000;
  
  // Initialize first row
  for (int k = 0; k < width; ++k) {
    const int j = k - band;
    prev[k] = (j >= 0 && j <= band) ? j : INF;
  }
  
  // DP computation
  for (int i = 1; i <= n; ++i) {
    const int j_min = std::max(1, i - band);
    const int j_max = std::min(m, i + band);
    
    for (int k = 0; k < width; ++k) {
      curr[k] = INF;
    }
    
    // Handle left boundary (deletion from a)
    const int k_start = j_min - i + band;
    if (j_min == i - band && i - band >= 0) {
      curr[k_start] = i;
    }
    
    for (int j = j_min; j <= j_max; ++j) {
      const int k = j - i + band;
      
      const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
      
      int best = INF;
      
      // Deletion (from previous row, same column)
      if (k >= 0 && k < width) {
        best = std::min(best, prev[k] + 1);
      }
      
      // Insertion (from current row, previous column)
      if (k > 0 && k - 1 < width) {
        best = std::min(best, curr[k - 1] + 1);
      }
      
      // Substitution (from previous row, previous column)
      if (k > 0 && k - 1 < width) {
        best = std::min(best, prev[k - 1] + cost);
      }
      
      curr[k] = best;
    }
    
    std::swap(prev, curr);
  }
  
  const int k_final = m - n + band;
  return (k_final >= 0 && k_final < width) ? prev[k_final] : INF;
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  uint64_t hash = 0;
  std::vector<int> prev, curr;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      const int d = banded_distance(lhs[i], rhs[i], max_band, prev, curr);
      hash ^= static_cast<uint64_t>(d + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}