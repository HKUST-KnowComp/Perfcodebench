#include "interface.h"
#include <cstdint>
#include <cstddef>

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

        // Process 4 words per iteration for better ILP
        for (; i + 3 < n; i += 4) {
            uint64_t w0 = left[i+0] & right[i+0];
            uint64_t w1 = left[i+1] & right[i+1];
            uint64_t w2 = left[i+2] & right[i+2];
            uint64_t w3 = left[i+3] & right[i+3];
            cnt += __builtin_popcountll(w0);
            cnt += __builtin_popcountll(w1);
            cnt += __builtin_popcountll(w2);
            cnt += __builtin_popcountll(w3);
        }

        // Handle remaining words
        for (; i < n; ++i) {
            cnt += __builtin_popcountll(left[i] & right[i]);
        }

        total = cnt;   // keep last iteration’s result
    }

    return total;
}