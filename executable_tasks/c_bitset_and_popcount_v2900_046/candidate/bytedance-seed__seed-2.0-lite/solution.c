#include "interface.h"
#include <stdint.h>

uint64_t run(const uint64_t* restrict left, const uint64_t* restrict right, size_t word_count, int iters) {
    uint64_t final_total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_total = 0;
        for (size_t i = 0; i < word_count; ++i) {
            const uint64_t and_word = left[i] & right[i];
            current_total += __builtin_popcountll(and_word);
        }
        final_total = current_total;
    }
    return final_total;
}