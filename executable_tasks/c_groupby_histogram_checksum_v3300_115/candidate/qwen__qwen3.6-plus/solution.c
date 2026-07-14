#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));
        
        const uint32_t* k = keys;
        const uint32_t* v = vals;
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            hist[k[0]] += v[0];
            hist[k[1]] += v[1];
            hist[k[2]] += v[2];
            hist[k[3]] += v[3];
            k += 4;
            v += 4;
        }
        for (; i < n; ++i) {
            hist[*k++] += *v++;
        }
        
        hash = 1469598103934665603ULL;
        const uint64_t* h = hist;
        uint32_t j = 0;
        for (; j + 3 < buckets; j += 4) {
            hash ^= (h[0] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (h[1] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (h[2] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (h[3] + 1ULL); hash *= 1099511628211ULL;
            h += 4;
        }
        for (; j < buckets; ++j) {
            hash ^= (*h++ + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    free(hist);
    return hash;
}