#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
    
    const uint32_t* li = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* ri = right_keys.data();
    const uint32_t* rv = right_values.data();
    const uint32_t* li_end = li + left_keys.size();
    const uint32_t* ri_end = ri + right_keys.size();
    
    uint64_t final_hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        const uint32_t* i = li;
        const uint32_t* v_i = lv;
        const uint32_t* j = ri;
        const uint32_t* v_j = rv;
        
        uint32_t cur_key = 0;
        uint32_t cur_val = 0;
        bool first = true;
        
        while (i < li_end && j < ri_end) {
            uint32_t k, v;
            if (*i < *j) {
                k = *i; v = *v_i; i++; v_i++;
            } else if (*i > *j) {
                k = *j; v = *v_j; j++; v_j++;
            } else {
                k = *j; v = *v_j; j++; v_j++;
                while (i < li_end && *i == k) { i++; v_i++; }
            }
            
            if (!first && k == cur_key) {
                cur_val = v;
            } else {
                if (!first) {
                    h ^= cur_key; h *= FNV_PRIME;
                    h ^= cur_val; h *= FNV_PRIME;
                }
                cur_key = k; cur_val = v; first = false;
            }
        }
        while (i < li_end) {
            uint32_t k = *i; uint32_t v = *v_i; i++; v_i++;
            if (!first && k == cur_key) { cur_val = v; }
            else {
                if (!first) { h ^= cur_key; h *= FNV_PRIME; h ^= cur_val; h *= FNV_PRIME; }
                cur_key = k; cur_val = v; first = false;
            }
        }
        while (j < ri_end) {
            uint32_t k = *j; uint32_t v = *v_j; j++; v_j++;
            if (!first && k == cur_key) { cur_val = v; }
            else {
                if (!first) { h ^= cur_key; h *= FNV_PRIME; h ^= cur_val; h *= FNV_PRIME; }
                cur_key = k; cur_val = v; first = false;
            }
        }
        if (!first) {
            h ^= cur_key; h *= FNV_PRIME;
            h ^= cur_val; h *= FNV_PRIME;
        }
        final_hash = h;
    }
    return final_hash;
}