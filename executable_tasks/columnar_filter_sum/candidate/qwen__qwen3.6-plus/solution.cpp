#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t* f = flags.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (f[i] && (k[i] & 3u) == 0u && k[i] < key_threshold) {
                current_sum += v[i];
            }
        }
        result = current_sum;
    }
    
    return result;
}