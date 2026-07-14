#include "interface.h"
#include <vector>
#include <cstddef>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* __restrict__ k_ptr = keys.data();
    const uint32_t* __restrict__ v_ptr = values.data();
    const uint8_t* __restrict__ f_ptr = flags.data();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t k = k_ptr[i];
            iter_sum += (f_ptr[i] != 0 && k < key_threshold && (k & 3u) == 0u) ? v_ptr[i] : 0;
        }
        sum = iter_sum;
    }
    return sum;
}
