#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (iters <= 0) return 0;
    
    // Allocate zero-initialized histogram once, not per iteration
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;
    
    // Preallocate list to track modified bucket indices (max unique keys = n)
    uint32_t* modified = (uint32_t*)malloc(n * sizeof(uint32_t));
    if (!modified) {
        free(hist);
        return 0;
    }

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t mod_count = 0;
        // Build histogram, only track buckets we actually update
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = keys[i];
            if (hist[k] == 0) {
                modified[mod_count++] = k;
            }
            hist[k] += vals[i];
        }
        // Compute exact same FNV checksum as original baseline
        final_hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            final_hash ^= (hist[i] + 1ULL);
            final_hash *= 1099511628211ULL;
        }
        // Reset only modified buckets to zero for next iteration
        for (size_t m = 0; m < mod_count; ++m) {
            hist[modified[m]] = 0;
        }
    }

    free(modified);
    free(hist);
    return final_hash;
}