#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const int64_t k = 0x9E3779B1LL;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t local = 0;
        size_t i = 0;
        for (; i + 8 <= count; i += 8) {
            current += (int64_t)deltas[i];     local += (current + k);
            current += (int64_t)deltas[i+1]; local += (current + k);
            current += (int64_t)deltas[i+2]; local += (current + k);
            current += (int64_t)deltas[i+3]; local += (current + k);
            current += (int64_t)deltas[i+4]; local += (current + k);
            current += (int64_t)deltas[i+5]; local += (current + k);
            current += (int64_t)deltas[i+6]; local += (current + k);
            current += (int64_t)deltas[i+7]; local += (current + k);
        }
        for (; i < count; ++i) {
            current += (int64_t)deltas[i];
            local += (current + k);
        }
        total = local;
    }
    return total;
}