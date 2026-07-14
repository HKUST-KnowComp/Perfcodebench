#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    const size_t n = left_keys.size();
    const size_t m = right_keys.size();
    uint64_t hash = 0;
    
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    const uint32_t* le = lk + n;
    const uint32_t* re = rk + m;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const uint32_t* i = lk;
        const uint32_t* iv = lv;
        const uint32_t* j = rk;
        const uint32_t* jv = rv;
        
        while (i < le || j < re) {
            uint32_t k;
            if (j == re || (i < le && *i < *j)) {
                k = *i;
            } else {
                k = *j;
            }
            
            uint32_t val = 0;
            while (i < le && *i == k) {
                val = *iv++;
                i++;
            }
            while (j < re && *j == k) {
                val = *jv++;
                j++;
            }
            
            hash ^= k;
            hash *= 1099511628211ULL;
            hash ^= val;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}