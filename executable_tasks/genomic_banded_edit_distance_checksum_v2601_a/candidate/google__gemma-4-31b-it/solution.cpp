#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Optimized banded edit distance
int banded_distance(const std::string& a, const std::string& b, int k) {
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    
    // If length difference exceeds band, it's impossible to stay within band
    // However, the task implies max_band is a constraint on the search, 
    // and we must return the distance within that band or the standard distance.
    // In standard banded edit distance, if |n-m| > k, distance is at least |n-m|.
    if (std::abs(n - m) > k) return std::abs(n - m);

    // We use two rows to save space: prev and curr
    // The band is centered around the diagonal. 
    // For a given i, we only care about j such that |i - j| <= k.
    std::vector<int> prev(2 * k + 1, 0);
    std::vector<int> curr(2 * k + 1, 0);

    // Initial state: i = 0
    // dp[0][j] = j. Only j in [0, k] are within band.
    for (int j = 0; j <= k; ++j) {
        prev[k + j] = j;
    }
    // Indices outside the initial band are effectively infinity
    for (int j = k + 1; j <= 2 * k; ++j) {
        prev[j] = 1e9;
    }

    for (int i = 1; i <= n; ++i) {
        int start_j = std::max(1, i - k);
        int end_j = std::min(m, i + k);

        // Handle the left boundary of the band for the current row
        // dp[i][j] where j < start_j is effectively infinity
        // But we need to handle dp[i][0] if it's within the band
        int row_start_idx = k + (0 - i); // relative to center
        // Since we only store 2k+1 elements, we map j to (j - i + k)

        // Base case for the row: dp[i][0] = i if 0 is in band
        if (i <= k) {
            // The index for j=0 is k + (0 - i)
            // But our vector is size 2k+1, indices [0, 2k].
            // If i=1, j=0 is index k-1. 
        }

        for (int j = start_j; j <= end_j; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            
            // Substitution: dp[i-1][j-1]
            // j-1 relative to i-1 is (j-1) - (i-1) + k = j - i + k
            int res = prev[j - i + k] + cost;

            // Deletion: dp[i-1][j]
            // j relative to i-1 is j - (i-1) + k = j - i + k + 1
            if (j - i + k + 1 <= 2 * k) {
                res = std::min(res, prev[j - i + k + 1] + 1);
            }

            // Insertion: dp[i][j-1]
            // j-1 relative to i is (j-1) - i + k = j - i + k - 1
            if (j - i + k - 1 >= 0) {
                res = std::min(res, curr[j - i + k - 1] + 1);
            }

            curr[j - i + k] = res;
        }

        // Special case for dp[i][0] if it's within band
        if (i <= k) {
            curr[0 - i + k] = i;
        }

        prev.swap(curr);
    }

    int final_idx = m - n + k;
    return prev[final_idx];
}

// A more robust and faster banded implementation
int fast_banded_dist(const std::string& a, const std::string& b, int k) {
    int n = a.size();
    int m = b.size();
    if (std::abs(n - m) > k) return std::abs(n - m);

    // Use a fixed size buffer to avoid allocations in the loop
    // Max band is usually small, but we use vectors for safety
    static thread_local std::vector<int> dp_prev;
    static thread_local std::vector<int> dp_curr;
    dp_prev.assign(2 * k + 1, 1e9);
    dp_curr.assign(2 * k + 1, 1e9);

    // i = 0
    for (int j = 0; j <= k; ++j) dp_prev[k + j] = j;

    for (int i = 1; i <= n; ++i) {
        int j_min = std::max(1, i - k);
        int j_max = std::min(m, i + k);

        if (i <= k) dp_curr[k - i] = i;

        for (int j = j_min; j <= j_max; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            int res = dp_prev[k + j - i] + cost; // sub
            if (k + j - i + 1 <= 2 * k) res = std::min(res, dp_prev[k + j - i + 1] + 1); // del
            if (k + j - i - 1 >= 0) res = std::min(res, dp_curr[k + j - i - 1] + 1); // ins
            dp_curr[k + j - i] = res;
        }
        
        // Clear values that will fall out of band for the next row
        // to prevent pollution from previous i
        if (i + k < 2 * k + 1) { /* logic to clear */ }
        // Actually, just swapping and carefully managing boundaries is enough
        dp_prev = dp_curr;
        std::fill(dp_curr.begin(), dp_curr.end(), 1e9);
    }

    return dp_prev[k + m - n];
}

} // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
    
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < lhs.size(); ++i) {
            // Use the optimized banded distance
            int d = fast_banded_dist(lhs[i], rhs[i], max_band);
            hash ^= static_cast<uint64_t>(d + 1);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    return final_hash;
}
