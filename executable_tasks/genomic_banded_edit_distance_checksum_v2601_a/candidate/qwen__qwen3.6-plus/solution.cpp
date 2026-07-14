#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr int INF = 1000000000;

int banded_distance(const std::string& a, const std::string& b, int band) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (n == 0) return m;
    if (m == 0) return n;
    
    int diff = n - m;
    if (diff < 0) diff = -diff;
    if (diff > band) {
        std::vector<int> dp(m + 1);
        for (int j = 0; j <= m; ++j) dp[j] = j;
        for (int i = 1; i <= n; ++i) {
            int prev_diag = dp[0];
            dp[0] = i;
            for (int j = 1; j <= m; ++j) {
                int temp = dp[j];
                int cost = (a[i-1] == b[j-1]) ? 0 : 1;
                int del = dp[j] + 1;
                int ins = dp[j-1] + 1;
                int sub = prev_diag + cost;
                dp[j] = std::min(del, std::min(ins, sub));
                prev_diag = temp;
            }
        }
        return dp[m];
    }

    std::vector<int> dp(m + 1);
    for (int j = 0; j <= m; ++j) dp[j] = j;

    for (int i = 1; i <= n; ++i) {
        int prev_diag = dp[0];
        dp[0] = i;
        int low = std::max(1, i - band);
        int high = std::min(m, i + band);
        
        if (low > 1) dp[low - 1] = INF;
        
        for (int j = low; j <= high; ++j) {
            int temp = dp[j];
            int cost = (a[i-1] == b[j-1]) ? 0 : 1;
            int del = dp[j] + 1;
            int ins = dp[j-1] + 1;
            int sub = prev_diag + cost;
            dp[j] = std::min(del, std::min(ins, sub));
            prev_diag = temp;
        }
        
        if (high < m) dp[high + 1] = INF;
    }
    return dp[m];
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
    
    std::vector<int> dists;
    dists.reserve(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        dists.push_back(banded_distance(lhs[i], rhs[i], max_band));
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int d : dists) {
            hash ^= static_cast<uint64_t>(d + 1);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}