#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/* persistent buffer, freed at exit */
static uint8_t* g_buf = NULL;
static size_t   g_cap = 0;

static void ensure_buf(size_t n)
{
    if (n > g_cap) {
        free(g_buf);
        g_buf = (uint8_t*)malloc(n);
        g_cap = n;
    }
}

/* 32-bit division by 255 using the fast (x+127)/255 trick */
static inline uint32_t div255(uint32_t x)
{
    return (x + 127u) / 255u;
}

uint64_t run(const uint8_t* src, const uint8_t* dst,
             int width, int height, int iters)
{
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes  = pixels * 4u;
    ensure_buf(bytes);
    uint8_t* out = g_buf;

    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s = src;
        const uint8_t* d = dst;
        uint8_t*       o = out;
        size_t         n = pixels;

        /* process 4 pixels per iteration */
        while (n >= 4) {
            /* pixel 0 */
            uint32_t a0 = s[3];
            uint32_t inv0 = 255u - a0;
            o[0] = (uint8_t)div255(s[0]*a0 + d[0]*inv0);
            o[1] = (uint8_t)div255(s[1]*a0 + d[1]*inv0);
            o[2] = (uint8_t)div255(s[2]*a0 + d[2]*inv0);
            o[3] = 255u;
            /* pixel 1 */
            uint32_t a1 = s[7];
            uint32_t inv1 = 255u - a1;
            o[4] = (uint8_t)div255(s[4]*a1 + d[4]*inv1);
            o[5] = (uint8_t)div255(s[5]*a1 + d[5]*inv1);
            o[6] = (uint8_t)div255(s[6]*a1 + d[6]*inv1);
            o[7] = 255u;
            /* pixel 2 */
            uint32_t a2 = s[11];
            uint32_t inv2 = 255u - a2;
            o[8]  = (uint8_t)div255(s[8]*a2  + d[8]*inv2);
            o[9]  = (uint8_t)div255(s[9]*a2  + d[9]*inv2);
            o[10] = (uint8_t)div255(s[10]*a2 + d[10]*inv2);
            o[11] = 255u;
            /* pixel 3 */
            uint32_t a3 = s[15];
            uint32_t inv3 = 255u - a3;
            o[12] = (uint8_t)div255(s[12]*a3 + d[12]*inv3);
            o[13] = (uint8_t)div255(s[13]*a3 + d[13]*inv3);
            o[14] = (uint8_t)div255(s[14]*a3 + d[14]*inv3);
            o[15] = 255u;

            s += 16;
            d += 16;
            o += 16;
            n -= 4;
        }
        /* tail */
        while (n--) {
            uint32_t a = s[3];
            uint32_t inv = 255u - a;
            o[0] = (uint8_t)div255(s[0]*a + d[0]*inv);
            o[1] = (uint8_t)div255(s[1]*a + d[1]*inv);
            o[2] = (uint8_t)div255(s[2]*a + d[2]*inv);
            o[3] = 255u;
            s += 4;
            d += 4;
            o += 4;
        }

        /* FNV-1a checksum inline */
        const uint8_t* p = out;
        size_t k = bytes;
        while (k--) {
            hash ^= (uint64_t)*p++;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}