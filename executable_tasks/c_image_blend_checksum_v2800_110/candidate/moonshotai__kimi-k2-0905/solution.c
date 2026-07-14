#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/* persistent scratch buffer, grown on demand, never shrunk */
static uint8_t* g_buf   = NULL;
static size_t   g_cap   = 0;

static uint64_t checksum_bytes(const uint8_t* data, size_t n)
{
    uint64_t h = 14695981039346656037ULL;
    for (size_t i = 0; i < n; ++i) {
        h ^= (uint64_t)data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t run(const uint8_t* src, const uint8_t* dst,
             int width, int height, int iters)
{
    const size_t pixels = (size_t)width * (size_t)height;
    const size_t nbytes = pixels * 4;

    /* grow buffer once if needed */
    if (g_cap < nbytes) {
        g_buf = (uint8_t*)realloc(g_buf, nbytes);
        g_cap = nbytes;
    }
    uint8_t* out = g_buf;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* hot loop: fully inlined, unrolled, no function calls */
        for (size_t i = 0; i < pixels; ++i) {
            const size_t ix = i * 4;
            const uint32_t a  = src[ix + 3];
            const uint32_t ia = 255u - a;

            const uint32_t r = ((uint32_t)src[ix]     * a + (uint32_t)dst[ix]     * ia + 127u) / 255u;
            const uint32_t g = ((uint32_t)src[ix + 1] * a + (uint32_t)dst[ix + 1] * ia + 127u) / 255u;
            const uint32_t b = ((uint32_t)src[ix + 2] * a + (uint32_t)dst[ix + 2] * ia + 127u) / 255u;

            out[ix]     = (uint8_t)r;
            out[ix + 1] = (uint8_t)g;
            out[ix + 2] = (uint8_t)b;
            out[ix + 3] = 255u;
        }
        hash = checksum_bytes(out, nbytes);
    }
    return hash;
}