#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters)
{
    const std::size_t n = values.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        std::size_t i = 0;

        // unroll 4x to hide multiply latency
        for (; i + 4 <= n; i += 4) {
            s0 += static_cast<uint64_t>(values[i  ]) * lengths[i  ];
            s1 += static_cast<uint64_t>(values[i+1]) * lengths[i+1];
            s2 += static_cast<uint64_t>(values[i+2]) * lengths[i+2];
            s3 += static_cast<uint64_t>(values[i+3]) * lengths[i+3];
        }
        // tail
        for (; i < n; ++i) {
            s0 += static_cast<uint64_t>(values[i]) * lengths[i];
        }
        total = s0 + s1 + s2 + s3;
    }
    return total;
}