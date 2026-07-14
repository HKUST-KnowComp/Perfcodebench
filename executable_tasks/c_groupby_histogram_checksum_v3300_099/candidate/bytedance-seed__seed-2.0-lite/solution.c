#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0 || iters == 0) return 0;
    
    // Allocate histogram once to avoid repeated malloc/free overhead
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;
    
    const size_t hist_total_bytes = buckets * sizeof(uint64_t);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Fast bulk zeroing instead of reallocating zeroed memory
        memset(hist, 0, hist_total_bytes);
        
        // Preserve original aggregation order for identical results
        for (size_t i = 0; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        // Exact same FNV-1a hash calculation as baseline to preserve checksum correctness
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            hash ^= (hist[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    free(hist);
    return final_hash;
}