#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    const std::size_t L = left_keys.size();
    const std::size_t R = right_keys.size();
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t final_hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        std::size_t i = 0, j = 0;
        uint32_t last_k = 0, last_v = 0;
        bool has_last = false;
        
        while (i < L || j < R) {
            uint32_t k, v;
            if (j == R || (i < L && lk[i] < rk[j])) {
                k = lk[i]; v = lv[i]; ++i;
            } else {
                k = rk[j]; v = rv[j]; ++j;
            }
            
            if (has_last && k == last_k) {
                last_v = v;
            } else {
                if (has_last) {
                    hash ^= last_k;
                    hash *= FNV_PRIME;
                    hash ^= last_v;
                    hash *= FNV_PRIME;
                }
                last_k = k;
                last_v = v;
                has_last = true;
            }
        }
        if (has_last) {
            hash ^= last_k;
            hash *= FNV_PRIME;
            hash ^= last_v;
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}