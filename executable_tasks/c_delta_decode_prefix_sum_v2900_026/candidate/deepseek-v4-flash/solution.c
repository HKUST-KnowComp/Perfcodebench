#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const uint64_t CONST = 0x9E3779B1ULL;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        int64_t current = 0;
        const int32_t* ptr = deltas;
        size_t n = count;
        while (n--) {
            current += *ptr++;
            sum += (uint64_t)current + CONST;
        }
        total = sum;
    }
    return total;
}
