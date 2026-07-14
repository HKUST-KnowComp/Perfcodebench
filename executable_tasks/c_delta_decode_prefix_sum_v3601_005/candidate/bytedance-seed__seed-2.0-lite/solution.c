#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t final_total = 0;
    const uint64_t magic_offset = (uint64_t)0x9E3779B1LL;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t iter_total = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            iter_total += (uint64_t)current + magic_offset;
        }
        final_total = iter_total;
    }
    return final_total;
}