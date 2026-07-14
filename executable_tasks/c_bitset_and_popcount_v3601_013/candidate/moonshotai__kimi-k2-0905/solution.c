#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>
static inline uint64_t popcnt_u64(uint64_t x) { return __popcnt64(x); }
#else
static inline uint64_t popcnt_u64(uint64_t x) { return __builtin_popcountll(x); }
#endif

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < word_count; ++i) {
            sum += popcnt_u64(left[i] & right[i]);
        }
        total = sum;
    }
    return total;
}