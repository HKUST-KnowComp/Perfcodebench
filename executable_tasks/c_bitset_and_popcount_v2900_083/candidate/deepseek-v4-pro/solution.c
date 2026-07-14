#include "interface.h"

#include <stdint.h>

static inline uint64_t popcount(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__) || (defined(__has_builtin) && __has_builtin(__builtin_popcountll))
    return __builtin_popcountll(x);
#else
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (iters == 0) return 0;
    uint64_t total = 0;
    for (size_t i = 0; i < word_count; ++i) {
        uint64_t word = left[i] & right[i];
        total += popcount(word);
    }
    return total;
}