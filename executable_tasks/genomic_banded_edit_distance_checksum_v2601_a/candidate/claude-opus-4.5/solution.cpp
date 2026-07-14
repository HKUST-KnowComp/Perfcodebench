#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

inline int banded_edit_distance(const char* __restrict__ a, int n,
                                 const char* __restrict__ b, int m,
                                 int max_band, int* __restrict__ prev, int* __restrict__ curr) {
    if (n > m) {
        std::swap(a, b);
        std::swap(n, m);
    }
    
    const int diff = m - n;
    if (diff > max_band) {
        return max_band + 1;
    }
    
    const int band = max_band;
    const int width = 2 * band + 1;
    
    for (int k = 0; k < width; ++k) {
        prev[k] = band + 1;
        curr[k] = band + 1;
    }
    prev[band] = 0;
    
    for (int i = 1; i <= n; ++i) {
        const int j_min = std::max(1, i - band);
        const int j_max = std::min(m, i + band);
        
        const char ai = a[i - 1];
        
        for (int k = 0; k < width; ++k) {
            curr[k] = band + 1;
        }
        
        for (int j = j_min; j <= j_max; ++j) {
            const int k = j - i + band;
            const int cost = (ai == b[j - 1]) ? 0 : 1;
            
            int val = band + 1;
            
            // substitution/match: prev[k]
            const int sub = prev[k] + cost;
            if (sub < val) val = sub;
            
            // deletion (i-1, j): prev[k+1]
            if (k + 1 < width) {
                const int del = prev[k + 1] + 1;
                if (del < val) val = del;
            }
            
            // insertion (i, j-1): curr[k-1]
            if (k > 0) {
                const int ins = curr[k - 1] + 1;
                if (ins < val) val = ins;
            }
            
            curr[k] = val;
        }
        
        std::swap(prev, curr);
    }
    
    const int final_k = m - n + band;
    return (final_k >= 0 && final_k < width) ? prev[final_k] : (band + 1);
}

inline int full_banded_distance(const char* __restrict__ a, int n,
                                 const char* __restrict__ b, int m,
                                 int max_band, int* __restrict__ dp) {
    if (n > m) {
        std::swap(a, b);
        std::swap(n, m);
    }
    
    const int diff = m - n;
    if (diff > max_band) {
        return max_band + 1;
    }
    
    const int cols = m + 1;
    
    // Initialize first row
    for (int j = 0; j <= m && j <= max_band; ++j) {
        dp[j] = j;
    }
    for (int j = max_band + 1; j <= m; ++j) {
        dp[j] = max_band + 1;
    }
    
    for (int i = 1; i <= n; ++i) {
        const int j_min = std::max(1, i - max_band);
        const int j_max = std::min(m, i + max_band);
        
        int* __restrict__ curr_row = dp + i * cols;
        const int* __restrict__ prev_row = dp + (i - 1) * cols;
        
        // Initialize cells outside band
        for (int j = 0; j < j_min; ++j) {
            curr_row[j] = max_band + 1;
        }
        if (j_min == 1 && i <= max_band) {
            curr_row[0] = i;
        } else if (j_min > 0) {
            curr_row[j_min - 1] = max_band + 1;
        }
        
        // Set left boundary
        if (i - max_band <= 0) {
            curr_row[0] = i;
        }
        
        const char ai = a[i - 1];
        
        for (int j = j_min; j <= j_max; ++j) {
            const int cost = (ai == b[j - 1]) ? 0 : 1;
            
            int val = prev_row[j - 1] + cost;  // substitution
            
            const int del = prev_row[j] + 1;
            if (del < val) val = del;
            
            const int ins = curr_row[j - 1] + 1;
            if (ins < val) val = ins;
            
            curr_row[j] = val;
        }
        
        for (int j = j_max + 1; j <= m; ++j) {
            curr_row[j] = max_band + 1;
        }
    }
    
    return dp[n * cols + m];
}

inline int optimized_banded_distance(const char* __restrict__ a, int n,
                                      const char* __restrict__ b, int m,
                                      int max_band, int* __restrict__ prev, int* __restrict__ curr) {
    if (n > m) {
        std::swap(a, b);
        std::swap(n, m);
    }
    
    const int diff = m - n;
    if (diff > max_band) {
        return max_band + 1;
    }
    
    // Two-row approach with direct j indexing
    const int INF = max_band + n + m + 1;
    
    // Initialize prev row (row 0)
    for (int j = 0; j <= m; ++j) {
        prev[j] = (j <= max_band) ? j : INF;
    }
    
    for (int i = 1; i <= n; ++i) {
        const int j_min = std::max(1, i - max_band);
        const int j_max = std::min(m, i + max_band);
        
        // Initialize curr[0] for this row
        curr[0] = (i <= max_band) ? i : INF;
        
        // Initialize cells before j_min
        for (int j = 1; j < j_min; ++j) {
            curr[j] = INF;
        }
        
        const char ai = a[i - 1];
        
        for (int j = j_min; j <= j_max; ++j) {
            const int cost = (ai == b[j - 1]) ? 0 : 1;
            
            int val = prev[j - 1] + cost;  // substitution
            
            const int del = prev[j] + 1;
            if (del < val) val = del;
            
            const int ins = curr[j - 1] + 1;
            if (ins < val) val = ins;
            
            curr[j] = val;
        }
        
        // Initialize cells after j_max
        for (int j = j_max + 1; j <= m; ++j) {
            curr[j] = INF;
        }
        
        std::swap(prev, curr);
    }
    
    return std::min(prev[m], max_band + 1);
}

}  // namespace

uint64_t batch_edit_distance_checksum(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    int max_band,
    int iters) {
    
    if (lhs.empty()) {
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
        }
        return hash;
    }
    
    // Find max string length for buffer allocation
    std::size_t max_len = 0;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        max_len = std::max(max_len, lhs[i].size());
        max_len = std::max(max_len, rhs[i].size());
    }
    
    // Allocate reusable buffers
    std::vector<int> prev_buf(max_len + 2);
    std::vector<int> curr_buf(max_len + 2);
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < lhs.size(); ++i) {
            const std::string& a = lhs[i];
            const std::string& b = rhs[i];
            
            const int d = optimized_banded_distance(
                a.data(), static_cast<int>(a.size()),
                b.data(), static_cast<int>(b.size()),
                max_band, prev_buf.data(), curr_buf.data());
            
            hash ^= static_cast<uint64_t>(d + 1);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}