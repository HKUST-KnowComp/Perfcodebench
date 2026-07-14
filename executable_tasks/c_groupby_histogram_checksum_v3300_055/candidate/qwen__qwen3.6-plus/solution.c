#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;
    
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;
    
    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;
    uint64_t* restrict h = hist;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        memset(h, 0, buckets * sizeof(uint64_t));
        
        for (size_t i = 0; i < n; ++i) {
            h[k[i]] += v[i];
        }
        
        hash = 1469598103934665603ULL;
        const uint64_t* p = h;
        const uint64_t* end = h + buckets;
        while (p < end) {
            hash ^= (*p++ + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    
    free(hist);
    return hash;
}