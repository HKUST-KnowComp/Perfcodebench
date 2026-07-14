#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
    
    uint64_t hash = 0;
    std::vector<int> dp;
    dp.reserve(2048);

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t idx = 0; idx < lhs.size(); ++idx) {
            const std::string& a = lhs[idx];
            const std::string& b = rhs[idx];
            int n = static_cast<int>(a.size());
            int m = static_cast<int>(b.size());
            
            if (n == 0) {
                hash ^= static_cast<uint64_t>(m + 1);
                hash *= 1099511628211ULL;
                continue;
            }
            if (m == 0) {
                hash ^= static_cast<uint64_t>(n + 1);
                hash *= 1099511628211ULL;
                continue;
            }
            
            int diff = n - m;
            if (diff < 0) diff = -diff;
            int eff_k = max_band > diff ? max_band : diff;
            
            int row_size = m + 1;
            if (static_cast<int>(dp.size()) < 2 * row_size) {
                dp.resize(2 * row_size);
            }
            int* prev = dp.data();
            int* curr = dp.data() + row_size;
            
            for (int j = 0; j <= m; ++j) prev[j] = j;
            
            const char* a_data = a.data();
            const char* b_data = b.data();
            
            for (int i = 1; i <= n; ++i) {
                int low = i - eff_k;
                if (low < 0) low = 0;
                int high = i + eff_k;
                if (high > m) high = m;
                
                if (low == 0) {
                    curr[0] = i;
                } else {
                    curr[low] = prev[low] + 1;
                }
                
                char a_char = a_data[i-1];
                for (int j = low + 1; j < high; ++j) {
                    int cost = (a_char == b_data[j-1]) ? 0 : 1;
                    int del = prev[j] + 1;
                    int ins = curr[j-1] + 1;
                    int sub = prev[j-1] + cost;
                    int best = del < ins ? del : ins;
                    if (sub < best) best = sub;
                    curr[j] = best;
                }
                
                if (high == m) {
                    int cost = (a_char == b_data[m-1]) ? 0 : 1;
                    int del = prev[m] + 1;
                    int ins = curr[m-1] + 1;
                    int sub = prev[m-1] + cost;
                    int best = del < ins ? del : ins;
                    if (sub < best) best = sub;
                    curr[m] = best;
                } else {
                    curr[high] = curr[high-1] + 1;
                }
                
                int* tmp = prev;
                prev = curr;
                curr = tmp;
            }
            
            int dist = prev[m];
            hash ^= static_cast<uint64_t>(dist + 1);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}