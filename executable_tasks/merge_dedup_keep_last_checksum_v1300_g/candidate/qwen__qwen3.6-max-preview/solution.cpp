#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    size_t n = left_keys.size();
    size_t m = right_keys.size();
    size_t max_sz = n + m;
    
    std::vector<uint32_t> out_keys(max_sz);
    std::vector<uint32_t> out_vals(max_sz);
    
    const uint32_t* lk_ptr = left_keys.data();
    const uint32_t* lv_ptr = left_values.data();
    const uint32_t* rk_ptr = right_keys.data();
    const uint32_t* rv_ptr = right_values.data();
    uint32_t* ok_ptr = out_keys.data();
    uint32_t* ov_ptr = out_vals.data();
    
    uint64_t hash = 0;
    const uint64_t fnv_prime = 1099511628211ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0, k = 0;
        
        while (i < n && j < m) {
            uint32_t lk = lk_ptr[i];
            uint32_t rk = rk_ptr[j];
            if (lk <= rk) {
                if (k > 0 && ok_ptr[k-1] == lk) {
                    ov_ptr[k-1] = lv_ptr[i];
                } else {
                    ok_ptr[k] = lk;
                    ov_ptr[k] = lv_ptr[i];
                    ++k;
                }
                ++i;
            } else {
                if (k > 0 && ok_ptr[k-1] == rk) {
                    ov_ptr[k-1] = rv_ptr[j];
                } else {
                    ok_ptr[k] = rk;
                    ov_ptr[k] = rv_ptr[j];
                    ++k;
                }
                ++j;
            }
        }
        while (i < n) {
            uint32_t lk = lk_ptr[i];
            if (k > 0 && ok_ptr[k-1] == lk) {
                ov_ptr[k-1] = lv_ptr[i];
            } else {
                ok_ptr[k] = lk;
                ov_ptr[k] = lv_ptr[i];
                ++k;
            }
            ++i;
        }
        while (j < m) {
            uint32_t rk = rk_ptr[j];
            if (k > 0 && ok_ptr[k-1] == rk) {
                ov_ptr[k-1] = rv_ptr[j];
            } else {
                ok_ptr[k] = rk;
                ov_ptr[k] = rv_ptr[j];
                ++k;
            }
            ++j;
        }
        
        hash = 1469598103934665603ULL;
        for (size_t idx = 0; idx < k; ++idx) {
            hash ^= ok_ptr[idx];
            hash *= fnv_prime;
            hash ^= ov_ptr[idx];
            hash *= fnv_prime;
        }
    }
    
    return hash;
}