#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    uint64_t hash = 0;
    const std::size_t n_left = left_keys.size();
    const std::size_t n_right = right_keys.size();
    
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        std::size_t i = 0, j = 0;
        bool has_last = false;
        uint32_t last_key = 0;
        uint32_t last_val = 0;
        
        while (i < n_left || j < n_right) {
            uint32_t k, v;
            if (j == n_right || (i < n_left && lk[i] <= rk[j])) {
                k = lk[i];
                v = lv[i];
                ++i;
            } else {
                k = rk[j];
                v = rv[j];
                ++j;
            }
            
            if (has_last && k == last_key) {
                last_val = v;
            } else {
                if (has_last) {
                    hash ^= last_key;
                    hash *= 1099511628211ULL;
                    hash ^= last_val;
                    hash *= 1099511628211ULL;
                }
                last_key = k;
                last_val = v;
                has_last = true;
            }
        }
        if (has_last) {
            hash ^= last_key;
            hash *= 1099511628211ULL;
            hash ^= last_val;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}