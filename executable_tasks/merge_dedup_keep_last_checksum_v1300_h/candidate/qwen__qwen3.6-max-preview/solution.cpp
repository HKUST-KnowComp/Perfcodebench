#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace {
inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}
}  // namespace

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
    const std::size_t n = left_keys.size();
    const std::size_t m = right_keys.size();
    
    uint64_t final_hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0, j = 0;
        uint64_t hash = 1469598103934665603ULL;
        
        while (i < n && j < m) {
            uint32_t k_left = lk[i];
            uint32_t k_right = rk[j];
            
            if (k_left < k_right) {
                uint32_t val = lv[i];
                i++;
                while (i < n && lk[i] == k_left) {
                    val = lv[i];
                    i++;
                }
                hash = mix(hash, k_left);
                hash = mix(hash, val);
            } else if (k_left > k_right) {
                uint32_t val = rv[j];
                j++;
                while (j < m && rk[j] == k_right) {
                    val = rv[j];
                    j++;
                }
                hash = mix(hash, k_right);
                hash = mix(hash, val);
            } else {
                // k_left == k_right: right side wins (keep-last semantics)
                i++;
                while (i < n && lk[i] == k_left) i++;
                
                uint32_t val = rv[j];
                j++;
                while (j < m && rk[j] == k_right) {
                    val = rv[j];
                    j++;
                }
                hash = mix(hash, k_right);
                hash = mix(hash, val);
            }
        }
        
        while (i < n) {
            uint32_t k = lk[i];
            uint32_t val = lv[i];
            i++;
            while (i < n && lk[i] == k) {
                val = lv[i];
                i++;
            }
            hash = mix(hash, k);
            hash = mix(hash, val);
        }
        
        while (j < m) {
            uint32_t k = rk[j];
            uint32_t val = rv[j];
            j++;
            while (j < m && rk[j] == k) {
                val = rv[j];
                j++;
            }
            hash = mix(hash, k);
            hash = mix(hash, val);
        }
        
        final_hash = hash;
    }
    
    return final_hash;
}