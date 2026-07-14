#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (iters <= 0) return 0;

    uint64_t *hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint32_t *touched = NULL;
    unsigned char *seen = NULL;

    if (buckets != 0 && n != 0) {
        touched = (uint32_t*)malloc(n * sizeof(uint32_t));
        seen = (unsigned char*)calloc((size_t)buckets, sizeof(unsigned char));
        if (!touched || !seen) {
            free(seen);
            free(touched);
            free(hist);
            return 0;
        }
    }

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t touched_count = 0;

        const uint32_t *k = keys;
        const uint32_t *v = vals;
        size_t i = 0;

        for (; i + 4 <= n; i += 4) {
            uint32_t k0 = k[i + 0];
            uint32_t k1 = k[i + 1];
            uint32_t k2 = k[i + 2];
            uint32_t k3 = k[i + 3];

            if (!seen[k0]) { seen[k0] = 1; touched[touched_count++] = k0; }
            if (!seen[k1]) { seen[k1] = 1; touched[touched_count++] = k1; }
            if (!seen[k2]) { seen[k2] = 1; touched[touched_count++] = k2; }
            if (!seen[k3]) { seen[k3] = 1; touched[touched_count++] = k3; }

            hist[k0] += v[i + 0];
            hist[k1] += v[i + 1];
            hist[k2] += v[i + 2];
            hist[k3] += v[i + 3];
        }
        for (; i < n; ++i) {
            uint32_t kk = k[i];
            if (!seen[kk]) { seen[kk] = 1; touched[touched_count++] = kk; }
            hist[kk] += v[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= 1099511628211ULL;
        }

        for (size_t t = 0; t < touched_count; ++t) {
            uint32_t idx = touched[t];
            hist[idx] = 0;
            seen[idx] = 0;
        }
    }

    free(seen);
    free(touched);
    free(hist);
    return hash;
}
