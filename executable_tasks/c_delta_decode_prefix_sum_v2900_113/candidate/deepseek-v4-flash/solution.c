#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t inner_total = 0;
    int64_t current = 0;
    const int64_t constant = 0x9E3779B1LL;
    for (size_t i = 0; i < count; ++i) {
        current += (int64_t)deltas[i];
        inner_total += (uint64_t)(current + constant);
    }
    return inner_total * (uint64_t)iters;
}
