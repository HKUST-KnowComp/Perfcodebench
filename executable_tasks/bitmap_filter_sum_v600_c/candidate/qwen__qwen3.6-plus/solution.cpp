#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (key_space == 0 || probe_keys.empty()) return 0;
    
    std::vector<uint8_t> bitmap(key_space, 0);
    uint64_t sum = 0;
    
    const uint32_t* pk = probe_keys.data();
    const uint32_t* val = values.data();
    const size_t n = probe_keys.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        for (uint32_t k : enabled_keys) {
            bitmap[k] = 1;
        }
        
        for (size_t i = 0; i < n; ++i) {
            sum += static_cast<uint64_t>(val[i]) * bitmap[pk[i]];
        }
        
        for (uint32_t k : enabled_keys) {
            bitmap[k] = 0;
        }
    }
    return sum;
}