#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (iters <= 0) return 0;
    uint64_t total = 0;
    uint64_t current = 0;
    const uint64_t constant = (uint64_t)(0x9E3779B1LL);
    for (size_t i = 0; i < count; ++i) {
        uint64_t delta = (uint64_t)(int64_t)deltas[i];
        current += delta;
        total += current + constant;
    }
    return total;
}