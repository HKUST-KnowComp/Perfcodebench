#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    size_t n = left_keys.size();
    size_t m = right_keys.size();
    
    uint64_t final_hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        if (n == 0 && m == 0) {
            final_hash = 1469598103934665603ULL;
            continue;
        }
        
        uint64_t hash = 1469598103934665603ULL;
        uint32_t last_key, last_val;
        size_t i = 0, j = 0;
        
        // Process first element to initialize state without branching inside the loop
        if (j == m || (i < n && lk[i] <= rk[j])) {
            last_key = lk[i];
            last_val = lv[i];
            i++;
        } else {
            last_key = rk[j];
            last_val = rv[j];
            j++;
        }
        
        while (i < n && j < m) {
            uint32_t k, v;
            if (lk[i] <= rk[j]) {
                k = lk[i]; v = lv[i]; i++;
            } else {
                k = rk[j]; v = rv[j]; j++;
            }
            if (k == last_key) {
                last_val = v;
            } else {
                hash ^= last_key;
                hash *= 1099511628211ULL;
                hash ^= last_val;
                hash *= 1099511628211ULL;
                last_key = k;
                last_val = v;
            }
        }
        while (i < n) {
            uint32_t k = lk[i]; uint32_t v = lv[i]; i++;
            if (k == last_key) {
                last_val = v;
            } else {
                hash ^= last_key;
                hash *= 1099511628211ULL;
                hash ^= last_val;
                hash *= 1099511628211ULL;
                last_key = k;
                last_val = v;
            }
        }
        while (j < m) {
            uint32_t k = rk[j]; uint32_t v = rv[j]; j++;
            if (k == last_key) {
                last_val = v;
            } else {
                hash ^= last_key;
                hash *= 1099511628211ULL;
                hash ^= last_val;
                hash *= 1099511628211ULL;
                last_key = k;
                last_val = v;
            }
        }
        
        hash ^= last_key;
        hash *= 1099511628211ULL;
        hash ^= last_val;
        hash *= 1099511628211ULL;
        
        final_hash = hash;
    }
    return final_hash;
}