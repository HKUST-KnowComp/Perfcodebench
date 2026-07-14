#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (__builtin_expect(iters <= 0, 0)) {
        return 0;
    }

    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t n = word_count;
    size_t i = 0;

    uint64_t t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0, t7 = 0;
    for (; i + 8 <= n; i += 8) {
        t0 += __builtin_popcountll(l[i]     & r[i]);
        t1 += __builtin_popcountll(l[i + 1] & r[i + 1]);
        t2 += __builtin_popcountll(l[i + 2] & r[i + 2]);
        t3 += __builtin_popcountll(l[i + 3] & r[i + 3]);
        t4 += __builtin_popcountll(l[i + 4] & r[i + 4]);
        t5 += __builtin_popcountll(l[i + 5] & r[i + 5]);
        t6 += __builtin_popcountll(l[i + 6] & r[i + 6]);
        t7 += __builtin_popcountll(l[i + 7] & r[i + 7]);
    }

    uint64_t total = t0 + t1 + t2 + t3 + t4 + t5 + t6 + t7;
    for (; i < n; ++i) {
        total += __builtin_popcountll(l[i] & r[i]);
    }

    return total;
}
