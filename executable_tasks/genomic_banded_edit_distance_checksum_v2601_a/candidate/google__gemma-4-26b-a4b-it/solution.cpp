#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace {

int banded_edit_distance(const std::string& a, const std::string& b, int k, std::vector<int>& dp_prev, std::vector<int>& dp_curr) {
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());

    if (std::abs(n - m) > k) return std::abs(n - m); // Though problem implies k is a bound for the distance

    // Standard DP with band constraint
    // dp[i][j] is only computed if |i - j| <= k
    // We use two rows to save space: dp_prev and dp_curr
    
    // Initialize dp_prev for i=0
    // dp[0][j] = j, but only for j <= k
    for (int j = 0; j <= m; ++j) {
        dp_prev[j] = j;
    }

    for (int i = 1; i <= n; ++i) {
        int j_start = std::max(1, i - k);
        int j_end = std::min(m, i + k);

        // Base case for the row: dp[i][0] = i if i <= k, else infinity
        if (i <= k) {
            dp_curr[0] = i;
        } else {
            dp_curr[0] = k + 1; // Represents infinity relative to band
        }

        // If the band starts after j=1, we need to ensure the cell before j_start is 'infinity'
        // However, since we only care about the band, we can just handle the boundaries.
        
        for (int j = 1; j <= m; ++j) {
            if (j >= j_start && j <= j_end) {
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                int res = dp_prev[j - 1] + cost; // substitution
                
                // deletion (from i-1, j)
                if (j >= i - k) {
                    int del = dp_prev[j] + 1;
                    if (del < res) res = del;
                } else {
                    res = k + 1; // effectively infinity
                }

                // insertion (from i, j-1)
                if (j - 1 >= i - k) {
                    int ins = dp_curr[j - 1] + 1;
                    if (ins < res) res = ins;
                } else {
                    // This part is tricky with the 1D optimization. 
                    // If j-1 is outside the band, we treat it as infinity.
                }
                
                // Re-calculating more robustly for the band:
                int sub = dp_prev[j-1] + cost;
                int del = (j <= i + k - 1 && j >= i - k) ? dp_prev[j] + 1 : k + 1;
                int ins = (j - 1 >= i - k && j - 1 <= i + k) ? dp_curr[j-1] + 1 : k + 1;
                
                // Corrected logic for banded DP
                // We must ensure that values outside the band are treated as infinity
                // to prevent them from being picked up by the min() operation.
            }
        }
    }
    return 0;
}

// A more reliable banded implementation using the property that we only care about |i-j| <= k
int compute_banded(const std::string& a, const std::string& b, int k, std::vector<int>& dp_prev, std::vector<int>& dp_curr) {
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    const int INF = n + m + 1;

    for (int j = 0; j <= m; ++j) dp_prev[j] = (j <= k) ? j : INF;

    for (int i = 1; i <= n; ++i) {
        int j_min = std::max(1, i - k);
        int j_max = std::min(m, i + k);

        dp_curr[0] = (i <= k) ? i : INF;

        // Fill cells before the band with INF to avoid contamination
        if (j_min > 1) dp_curr[j_min - 1] = INF;

        for (int j = j_min; j <= j_max; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            int sub = dp_prev[j - 1] + cost;
            int del = dp_prev[j] + 1;
            int ins = dp_curr[j - 1] + 1;
            
            int best = sub;
            if (del < best) best = del;
            if (ins < best) best = ins;
            dp_curr[j] = best;
        }
        
        // Fill cell after the band with INF
        if (j_max < m) dp_curr[j_max + 1] = INF;

        std::swap(dp_prev, dp_curr);
    }

    return dp_prev[m];
}

} // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
  uint64_t hash = 0;
  
  // Pre-allocate buffers to reuse
  size_t max_len = 0;
  for(size_t i=0; i<lhs.size(); ++i) {
      max_len = std::max({max_len, lhs[i].size(), rhs[i].size()});
  }
  std::vector<int> dp_prev(max_len + 1);
  std::vector<int> dp_curr(max_len + 1);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      const int d = compute_banded(lhs[i], rhs[i], max_band, dp_prev, dp_curr);
      hash ^= static_cast<uint64_t>(d + 1);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
