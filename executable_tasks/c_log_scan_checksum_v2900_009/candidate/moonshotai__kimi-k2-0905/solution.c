#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t kInit   = 1469598103934665603ULL;
    const uint64_t kPrime  = 1099511628211ULL;
    uint64_t h0 = kInit, h1 = kInit, h2 = kInit, h3 = kInit;

    size_t i = 0;
    for (; i + 4 <= row_count; i += 4) {
        const unsigned char *r0 = (const unsigned char *)rows[i+0];
        const unsigned char *r1 = (const unsigned char *)rows[i+1];
        const unsigned char *r2 = (const unsigned char *)rows[i+2];
        const unsigned char *r3 = (const unsigned char *)rows[i+3];

        /* ensure the fixed 15-byte format (compiler can hoist) */
        uint64_t v0 = (uint64_t)(r0[2] + r0[7]);
        uint64_t v1 = (uint64_t)(r1[2] + r1[7]);
        uint64_t v2 = (uint64_t)(r2[2] + r2[7]);
        uint64_t v3 = (uint64_t)(r3[2] + r3[7]);

        h0 ^= (v0 + 1ULL); h0 *= kPrime;
        h1 ^= (v1 + 1ULL); h1 *= kPrime;
        h2 ^= (v2 + 1ULL); h2 *= kPrime;
        h3 ^= (v3 + 1ULL); h3 *= kPrime;
    }

    /* horizontal reduction */
    uint64_t hash = ((h0 ^ h1 ^ h2 ^ h3) * kPrime) ^ kInit;

    /* tail (0–3 remaining rows) */
    for (; i < row_count; ++i) {
        const unsigned char *r = (const unsigned char *)rows[i];
        uint64_t v = (uint64_t)(r[2] + r[7]);
        hash ^= (v + 1ULL);
        hash *= kPrime;
    }
    return hash;
}