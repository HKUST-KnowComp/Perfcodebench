#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

int banded_edit_distance(const std::string& a, const std::string& b, int k) {
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    if (std::abs(n - m) > k) return -1; // Should not happen based on problem constraints

    // Use two rows to save memory
    std::vector<int> prev(m + 1);
    std::vector<int> curr(m + 1);

    for (int j = 0; j <= m; ++j) prev[j] = j;

    for (int i = 1; i <= n; ++i) {
        curr[0] = i;
        int start = std::max(1, i - k);
        int end = std::min(m, i + k);
        
        if (start > 1) curr[start - 1] = 1e9; // Out of band

        for (int j = start; j <= end; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            curr[j] = std::min({curr[j - 1] + 1, prev[j] + 1, prev[j - 1] + cost});
        }
        if (end < m) curr[end + 1] = 1e9;
        std::swap(prev, curr);
    }
    return prev[m];
}

} // namespace

uint64_t batch_edit_distance_checksum(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs, int max_band, int iters) {
    uint64_t hash = 0;
    const size_t n = lhs.size();
    std::vector<int> distances(n);

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            int d = banded_edit_distance(lhs[i], rhs[i], max_band);
            hash ^= static_cast<uint64_t>(d + 1);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}