#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (iters == 0 || count == 0) return 0;
    int64_t current = 0;
    uint64_t sum = 0;
    for (size_t i = 0; i < count; ++i) {
        current += (int64_t)deltas[i];
        sum += (uint64_t)(current + 0x9E3779B1LL);
    }
    return sum;
}