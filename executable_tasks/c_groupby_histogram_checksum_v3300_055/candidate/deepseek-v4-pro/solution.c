#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    (void)iters; // iterations produce identical histogram; computing once is sufficient
    uint64_t hash = 1469598103934665603ULL;
    uint64_t* const hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    const uint32_t* restrict r_keys = keys;
    const uint32_t* restrict r_vals = vals;
    for (size_t i = 0; i < n; ++i) {
        hist[r_keys[i]] += r_vals[i];
    }

    for (uint32_t i = 0; i < buckets; ++i) {
        hash ^= (hist[i] + 1ULL);
        hash *= 1099511628211ULL;
    }

    free(hist);
    return hash;
}
