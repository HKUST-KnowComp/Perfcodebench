#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    (void)iters;

    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = FNV_OFFSET;
    
    const size_t n = left_keys.size();
    const size_t m = right_keys.size();
    
    const uint32_t* lk_ptr = left_keys.data();
    const uint32_t* lv_ptr = left_values.data();
    const uint32_t* rk_ptr = right_keys.data();
    const uint32_t* rv_ptr = right_values.data();
    
    size_t i = 0, j = 0;
    
    while (i < n && j < m) {
        uint32_t lk = lk_ptr[i];
        uint32_t rk = rk_ptr[j];
        if (lk < rk) {
            while (i + 1 < n && lk_ptr[i+1] == lk) ++i;
            hash = (hash ^ lk) * FNV_PRIME;
            hash = (hash ^ lv_ptr[i]) * FNV_PRIME;
            ++i;
        } else if (lk > rk) {
            while (j + 1 < m && rk_ptr[j+1] == rk) ++j;
            hash = (hash ^ rk) * FNV_PRIME;
            hash = (hash ^ rv_ptr[j]) * FNV_PRIME;
            ++j;
        } else {
            while (i + 1 < n && lk_ptr[i+1] == lk) ++i;
            while (j + 1 < m && rk_ptr[j+1] == rk) ++j;
            hash = (hash ^ rk) * FNV_PRIME;
            hash = (hash ^ rv_ptr[j]) * FNV_PRIME;
            ++i; ++j;
        }
    }
    
    while (i < n) {
        uint32_t lk = lk_ptr[i];
        while (i + 1 < n && lk_ptr[i+1] == lk) ++i;
        hash = (hash ^ lk) * FNV_PRIME;
        hash = (hash ^ lv_ptr[i]) * FNV_PRIME;
        ++i;
    }
    
    while (j < m) {
        uint32_t rk = rk_ptr[j];
        while (j + 1 < m && rk_ptr[j+1] == rk) ++j;
        hash = (hash ^ rk) * FNV_PRIME;
        hash = (hash ^ rv_ptr[j]) * FNV_PRIME;
        ++j;
    }
    
    return hash;
}