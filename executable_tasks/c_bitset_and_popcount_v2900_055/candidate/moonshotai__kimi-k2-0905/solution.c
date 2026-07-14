#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/* portable popcount helpers */
static inline uint64_t popcount_u64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(x);
#elif defined(_MSC_VER)
    return __popcnt64(x);
#else
    /* fast SW fallback */
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc = 0;
        for (size_t i = 0; i < word_count; ++i) {
            acc += popcount_u64(left[i] & right[i]);
        }
        total = acc;
    }
    return total;
}