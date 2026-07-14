#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    if (iters <= 0 || keys.empty()) return 0;

    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const uint8_t* f_ptr = flags.data();

    uint64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
            sum += v_ptr[i];
        }
    }
    return sum;
}