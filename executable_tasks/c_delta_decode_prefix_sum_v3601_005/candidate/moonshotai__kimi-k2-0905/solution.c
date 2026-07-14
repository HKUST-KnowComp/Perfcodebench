#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0x9E3779B1LL;          // start with the constant already added
        uint64_t acc = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            acc += (uint64_t)current;
        }
        total = acc;
    }
    return total;
}