#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint8_t* f = flags.data();
    const uint32_t* p = payloads.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (f[i] && k[i] < threshold && (k[i] & 7u) == 0u) {
                iter_sum += p[i];
            }
        }
        result = iter_sum;
    }
    return result;
}