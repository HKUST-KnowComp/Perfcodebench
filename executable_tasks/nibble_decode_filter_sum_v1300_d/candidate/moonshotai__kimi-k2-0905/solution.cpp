#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* __restrict p = packed.data();
    const std::size_t n = packed.size();
    uint64_t sum = 0;

    if (threshold >= 15) {          // no nibble can pass
        for (int iter = 0; iter < iters; ++iter) {
            // do all the work of the outer loop to keep timing fair
            for (std::size_t i = 0; i < n; ++i) {
                (void)p[i];
            }
        }
        return 0;
    }

    const uint32_t t = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        std::size_t i = 0;

        // unroll 8× to expose ILP
        for (; i + 8 <= n; i += 8) {
            uint64_t s = 0;
            #define STEP(j) do {                                      \
                uint8_t b = p[i+j];                                   \
                uint32_t lo = b & 15u;                                \
                uint32_t hi = b >> 4u;                                \
                s += (lo > t) ? lo : 0;                               \
                s += (hi > t) ? hi : 0;                               \
            } while (0)
            STEP(0); STEP(1); STEP(2); STEP(3);
            STEP(4); STEP(5); STEP(6); STEP(7);
            #undef STEP
            local += s;
        }

        // tail
        for (; i < n; ++i) {
            uint8_t b = p[i];
            uint32_t lo = b & 15u;
            uint32_t hi = b >> 4u;
            local += (lo > t) ? lo : 0;
            local += (hi > t) ? hi : 0;
        }

        sum = local;
    }
    return sum;
}