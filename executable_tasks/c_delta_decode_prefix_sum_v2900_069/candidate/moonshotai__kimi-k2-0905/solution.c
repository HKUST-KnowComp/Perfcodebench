#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    const uint64_t k = 0x9E3779B1ULL;
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = 0;
        total = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (uint32_t)deltas[i];   /* zero-extend 32->64 */
            total += current + k;
        }
    }
    return total;
}