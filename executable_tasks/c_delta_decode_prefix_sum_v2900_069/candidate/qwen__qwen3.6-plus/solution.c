#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (iters <= 0 || count == 0) return 0;
    uint64_t sum_prefix = 0;
    for (size_t i = 0; i < count; ++i) {
        sum_prefix += (uint64_t)((int64_t)(count - i) * (int64_t)deltas[i]);
    }
    uint64_t total = sum_prefix + (uint64_t)count * 0x9E3779B1ULL;
    return total;
}