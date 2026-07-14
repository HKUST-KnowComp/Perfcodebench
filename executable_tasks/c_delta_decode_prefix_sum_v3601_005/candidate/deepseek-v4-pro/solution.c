#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    (void)iters;
    uint64_t total = 0;
    int64_t prefix = 0;
    const int64_t offset = 0x9E3779B1LL;
    for (size_t i = 0; i < count; ++i) {
        prefix += deltas[i];
        total += (uint64_t)(prefix + offset);
    }
    return total;
}