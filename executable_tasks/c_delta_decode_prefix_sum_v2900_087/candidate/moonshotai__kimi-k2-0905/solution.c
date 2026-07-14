#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0 || iters == 0) return 0;

    /* allocate once, reuse across iterations */
    int64_t* values = (int64_t*)malloc(count * sizeof(int64_t));
    if (!values) return 0;

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t acc   = 0;
        const int32_t* d = deltas;
        int64_t* v = values;
        size_t n = count;

        while (n--) {
            current += (int64_t)(*d++);
            *v++ = current;
            acc += (uint64_t)(current + 0x9E3779B1LL);
        }
        total = acc;
    }
    free(values);
    return total;
}