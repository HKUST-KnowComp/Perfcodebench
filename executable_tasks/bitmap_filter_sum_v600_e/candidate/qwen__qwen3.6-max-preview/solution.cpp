#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    std::vector<uint8_t> bitmap(key_space, 0);
    uint64_t sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* pk = probe_keys.data();
    const uint32_t* val = values.data();
    const uint32_t* ek = enabled_keys.data();
    const size_t ek_n = enabled_keys.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(bitmap.data(), 0, key_space);
        
        for (size_t i = 0; i < ek_n; ++i) {
            bitmap[ek[i]] = 1;
        }
        
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (bitmap[pk[i]]) {
                sum += val[i];
            }
        }
    }
    return sum;
}