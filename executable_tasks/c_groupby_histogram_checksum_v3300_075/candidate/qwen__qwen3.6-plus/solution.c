#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;
    
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;
    
    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;
    uint64_t* restrict h = hist;
    
    uint64_t hash = 0;
    size_t hist_bytes = (size_t)buckets * sizeof(uint64_t);
    
    for (int iter = 0; iter < iters; ++iter) {
        memset(h, 0, hist_bytes);
        
        for (size_t i = 0; i < n; ++i) {
            h[k[i]] += v[i];
        }
        
        hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            hash ^= (h[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    
    free(hist);
    return hash;
}