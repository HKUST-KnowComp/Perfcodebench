#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const std::size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t* f = flags.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (f[i] && k[i] < key_threshold && (k[i] & 3u) == 0u) {
                sum += v[i];
            }
        }
        result = sum;
    }
    
    return result;
}