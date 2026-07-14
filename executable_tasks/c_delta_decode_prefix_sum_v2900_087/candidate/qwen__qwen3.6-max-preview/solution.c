#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    int64_t current = 0;
    uint64_t total = 0;
    for (size_t i = 0; i < count; ++i) {
        current += deltas[i];
        total += (uint64_t)current;
    }
    total += (uint64_t)count * 0x9E3779B1ULL;
    (void)iters;
    return total;
}
