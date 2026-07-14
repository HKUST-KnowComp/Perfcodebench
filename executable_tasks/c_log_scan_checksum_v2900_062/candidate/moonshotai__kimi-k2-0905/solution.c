#include "interface.h"
#include <stdint.h>
#include <string.h>

/*
 * Fixed format: 15-byte row "ccNNNN-NNNNNNNN" + '\0'
 * We load 16 bytes, check the two fixed delimiters in parallel,
 * extract the two variable fields with shifts/masks, and compute
 * the required checksum contribution without branches.
 */

uint64_t run(const char* const* rows, size_t row_count)
{
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;

    uint64_t hash = FNV_OFFSET;

    for (size_t i = 0; i < row_count; ++i) {
        /* 16-byte load (safe: input is NUL-terminated) */
        uint64_t v0, v1;
        memcpy(&v0, rows[i]     , 8);
        memcpy(&v1, rows[i] + 8, 8);

        /* check fixed delimiters: rows[i][2]=='-' && rows[i][7]=='-' */
        const uint64_t dash = 0x2D2D2D2D2D2D2D2DULL;
        uint64_t want_dash  = ((v0 & 0x000000FF00000000ULL) == (dash & 0x000000FF00000000ULL)) &
                              ((v0 & 0x0000000000FF0000ULL) == (dash & 0x0000000000FF0000ULL));

        /* extract 4-digit field (ASCII digits) */
        uint32_t dig = (uint32_t)(v0 >> 24);          /* bytes 3-6 in little-endian order */
        uint32_t val4 = 0;
        val4 += (dig & 0xFFU) - '0';  dig >>= 8;
        val4 *= 10;
        val4 += (dig & 0xFFU) - '0';  dig >>= 8;
        val4 *= 10;
        val4 += (dig & 0xFFU) - '0';  dig >>= 8;
        val4 *= 10;
        val4 += (dig & 0xFFU) - '0';

        /* extract 8-digit hex field (ASCII hex) */
        uint64_t hex = (v0 >> 40) | (v1 << 24);
        uint64_t val8 = 0;
        for (int k = 0; k < 8; ++k) {
            uint8_t c = (uint8_t)hex; hex >>= 8;
            uint8_t n = c - '0';
            if (c >= 'A') n = c - 'A' + 10;
            val8 = (val8 << 4) | n;
        }

        /* combine into the required v = (byte[2] + byte[7]) */
        uint8_t b2 = (uint8_t)(v0 >> 16);
        uint8_t b7 = (uint8_t)(v0 >> 40);
        uint64_t v = (uint64_t)(b2 + b7);

        /* if format invalid, force v=0 */
        v &= -(int64_t)want_dash;

        /* FNV-1a step */
        hash ^= (v + 1ULL);
        hash *= FNV_PRIME;
    }
    return hash;
}