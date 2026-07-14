#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// Optimized blend using fast integer approximation for division by 255
// (x + 1 + (x >> 8)) >> 8 is a common trick for (x + 127) / 255
static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val + (val >> 8)) >> 8);
}

static uint64_t checksum_bytes_fast(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    // Process 8 bytes at a time to reduce loop iterations
    size_t i = 0;
    for (; i + 7 < n; i += 8) {
        uint64_t chunk;
        // Manual unrolling/loading to avoid alignment issues if necessary,
        // but standard memcpy is usually optimized by compiler to a single load.
        // For simplicity and correctness with potential unaligned access:
        chunk = *(const uint64_t*)(data + i);
        
        // We must process byte by byte to maintain the exact XOR-multiply order
        // of the original algorithm. However, we can unroll the loop.
        for(int j = 0; j < 8; ++j) {
            hash ^= (uint64_t)data[i + j];
            hash *= prime;
        }
    }
    
    for (; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    return hash;
}

// Re-implementing checksum to be strictly identical to original logic but unrolled
static uint64_t checksum_bytes_exact(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    // Allocate buffer once outside the loop to avoid malloc/free overhead
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            
            // Inline the blend logic for speed
            uint32_t r_val = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * (255u - a) + 127u;
            out[idx] = (uint8_t)((r_val + (r_val >> 8)) >> 8);
            
            uint32_t g_val = (uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * (255u - a) + 127u;
            out[idx + 1u] = (uint8_t)((g_val + (g_val >> 8)) >> 8);
            
            uint32_t b_val = (uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * (255u - a) + 127u;
            out[idx + 2u] = (uint8_t)((b_val + (b_val >> 8)) >> 8);
            
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes_exact(out, total_bytes);
    }

    free(out);
    return hash;
}