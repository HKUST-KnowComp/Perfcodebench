#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    const size_t nL = left_keys.size();
    const size_t nR = right_keys.size();
    
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    
    const uint32_t* le = lk + nL;
    const uint32_t* re = rk + nR;
    
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        const uint32_t* i = lk;
        const uint32_t* iv = lv;
        const uint32_t* j = rk;
        const uint32_t* jv = rv;
        
        while (i < le && j < re) {
            uint32_t k1 = *i;
            uint32_t k2 = *j;
            if (k1 < k2) {
                hash ^= k1; hash *= FNV_PRIME;
                hash ^= *iv; hash *= FNV_PRIME;
                ++i; ++iv;
            } else if (k1 > k2) {
                hash ^= k2; hash *= FNV_PRIME;
                hash ^= *jv; hash *= FNV_PRIME;
                ++j; ++jv;
            } else {
                hash ^= k2; hash *= FNV_PRIME;
                hash ^= *jv; hash *= FNV_PRIME;
                ++i; ++iv; ++j; ++jv;
            }
        }
        while (i < le) {
            hash ^= *i; hash *= FNV_PRIME;
            hash ^= *iv; hash *= FNV_PRIME;
            ++i; ++iv;
        }
        while (j < re) {
            hash ^= *j; hash *= FNV_PRIME;
            hash ^= *jv; hash *= FNV_PRIME;
            ++j; ++jv;
        }
    }
    return hash;
}