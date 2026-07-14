#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    uint64_t n = (uint64_t)count;
    uint64_t constant_term = n * 0x9E3779B1ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        uint64_t i = 0;
        for (; i + 3 < n; i += 4) {
            s0 += (uint64_t)((int64_t)deltas[i] * (int64_t)(n - i));
            s1 += (uint64_t)((int64_t)deltas[i+1] * (int64_t)(n - i - 1));
            s2 += (uint64_t)((int64_t)deltas[i+2] * (int64_t)(n - i - 2));
            s3 += (uint64_t)((int64_t)deltas[i+3] * (int64_t)(n - i - 3));
        }
        for (; i < n; ++i) {
            s0 += (uint64_t)((int64_t)deltas[i] * (int64_t)(n - i));
        }
        total = s0 + s1 + s2 + s3 + constant_term;
    }
    return total;
}