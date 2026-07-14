#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        int64_t cur = 0;
        for (size_t i = 0; i < count; ++i) {
            cur += (int64_t)deltas[i];
            s += (uint64_t)(cur + 0x9E3779B1LL);
        }
        total = s;
    }
    return total;
}