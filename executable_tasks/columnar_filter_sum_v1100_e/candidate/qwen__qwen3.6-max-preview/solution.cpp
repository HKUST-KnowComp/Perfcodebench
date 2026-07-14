#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t* f = flags.data();
    const std::size_t n = keys.size();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t ki = k[i];
            uint32_t pass = (f[i] != 0) & (ki < key_threshold) & ((ki & 3u) == 0u);
            s += v[i] & (0u - pass);
        }
        sum = s;
    }
    return sum;
}
