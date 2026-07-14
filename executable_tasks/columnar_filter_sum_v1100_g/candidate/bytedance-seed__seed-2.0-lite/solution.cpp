#include "interface.h"
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    const std::size_t N = keys.size();
    const uint32_t* __restrict__ k_ptr = keys.data();
    const uint32_t* __restrict__ v_ptr = values.data();
    const uint8_t* __restrict__ f_ptr = flags.data();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            if (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3U) == 0U) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
    }
    return sum;
}