#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        for (size_t i = 0; i < word_count; ++i) {
            uint64_t w = left[i] & right[i];
            cnt += (uint64_t)__builtin_popcountll(w);
        }
        total = cnt;
    }
    return total;
}