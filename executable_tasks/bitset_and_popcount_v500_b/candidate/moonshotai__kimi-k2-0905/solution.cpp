#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters)
{
    const std::size_t n = left.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        std::size_t i = 0;

        // 4-way unroll to hide popcnt latency
        for (; i + 3 < n; i += 4) {
            cnt += __builtin_popcountll(left[i  ] & right[i  ]);
            cnt += __builtin_popcountll(left[i+1] & right[i+1]);
            cnt += __builtin_popcountll(left[i+2] & right[i+2]);
            cnt += __builtin_popcountll(left[i+3] & right[i+3]);
        }
        // tail
        for (; i < n; ++i) {
            cnt += __builtin_popcountll(left[i] & right[i]);
        }
        total = cnt;   // keep final iteration’s result
    }
    return total;
}