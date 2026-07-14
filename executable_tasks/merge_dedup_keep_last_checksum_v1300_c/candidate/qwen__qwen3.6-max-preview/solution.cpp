#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    uint64_t hash = 0;
    const std::size_t n = left_keys.size();
    const std::size_t m = right_keys.size();
    
    const uint32_t* lk_ptr = left_keys.data();
    const uint32_t* lv_ptr = left_values.data();
    const uint32_t* rk_ptr = right_keys.data();
    const uint32_t* rv_ptr = right_values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        std::size_t i = 0, j = 0;
        
        uint32_t prev_key = 0;
        uint32_t prev_val = 0;
        bool has_prev = false;
        
        while (i < n && j < m) {
            uint32_t lk = lk_ptr[i];
            uint32_t rk = rk_ptr[j];
            uint32_t k, v;
            if (lk <= rk) {
                k = lk; v = lv_ptr[i]; ++i;
            } else {
                k = rk; v = rv_ptr[j]; ++j;
            }
            
            if (!has_prev) {
                prev_key = k; prev_val = v; has_prev = true;
            } else if (k == prev_key) {
                prev_val = v;
            } else {
                hash ^= prev_key; hash *= 1099511628211ULL;
                hash ^= prev_val; hash *= 1099511628211ULL;
                prev_key = k; prev_val = v;
            }
        }
        
        while (i < n) {
            uint32_t k = lk_ptr[i];
            uint32_t v = lv_ptr[i];
            ++i;
            if (!has_prev) {
                prev_key = k; prev_val = v; has_prev = true;
            } else if (k == prev_key) {
                prev_val = v;
            } else {
                hash ^= prev_key; hash *= 1099511628211ULL;
                hash ^= prev_val; hash *= 1099511628211ULL;
                prev_key = k; prev_val = v;
            }
        }
        
        while (j < m) {
            uint32_t k = rk_ptr[j];
            uint32_t v = rv_ptr[j];
            ++j;
            if (!has_prev) {
                prev_key = k; prev_val = v; has_prev = true;
            } else if (k == prev_key) {
                prev_val = v;
            } else {
                hash ^= prev_key; hash *= 1099511628211ULL;
                hash ^= prev_val; hash *= 1099511628211ULL;
                prev_key = k; prev_val = v;
            }
        }
        
        if (has_prev) {
            hash ^= prev_key; hash *= 1099511628211ULL;
            hash ^= prev_val; hash *= 1099511628211ULL;
        }
    }
    return hash;
}
