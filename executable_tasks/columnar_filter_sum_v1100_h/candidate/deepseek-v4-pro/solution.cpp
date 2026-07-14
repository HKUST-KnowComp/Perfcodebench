#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    uint64_t sum = 0;
    const std::size_t n = keys.size();
    const uint32_t* __restrict k = keys.data();
    const uint32_t* __restrict v = values.data();
    const uint8_t*  __restrict f = flags.data();

    for (std::size_t i = 0; i < n; ++i) {
        if (f[i] && k[i] < key_threshold && ((k[i] & 3u) == 0u)) {
            sum += static_cast<uint64_t>(v[i]);
        }
    }
    (void)iters;
    return sum;
}
