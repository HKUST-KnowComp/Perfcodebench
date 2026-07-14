#include "interface.h"

#include <stdint.h>
#include <string.h>

/* portable unaligned load */
static inline uint64_t load_u64(const void *p) {
    uint64_t v;
    memcpy(&v, p, sizeof v);
    return v;
}

static inline uint64_t parse_dec_u32(const char *s, const char **end) {
    uint64_t v = 0;
    for (;;) {
        uint64_t chunk = load_u64(s);
        uint64_t t = chunk - 0x3030303030303030ULL;   /* subtract '0' */
        uint64_t mask = ((t & 0x8080808080808080ULL) >> 7) ^ 0x0101010101010101ULL;
        if (mask != 0) {                /* found non-digit */
            uint32_t digit;
            while ((digit = (uint8_t)*s - '0') <= 9) {
                v = v * 10 + digit;
                ++s;
            }
            break;
        }
        /* fast path: 8 decimal digits */
        v = v * 100000000ULL +
            ((chunk >>  0) & 0xFF) * 10000000ULL +
            ((chunk >>  8) & 0xFF) * 1000000ULL  +
            ((chunk >> 16) & 0xFF) * 100000ULL   +
            ((chunk >> 24) & 0xFF) * 10000ULL    +
            ((chunk >> 32) & 0xFF) * 1000ULL     +
            ((chunk >> 40) & 0xFF) * 100ULL      +
            ((chunk >> 48) & 0xFF) * 10ULL       +
            ((chunk >> 56) & 0xFF) * 1ULL;
        s += 8;
    }
    *end = s;
    return v;
}

static inline int match_hot(const char *s) {
    /* 'h'|'o'|'t'|0  = 0x00746f68 in little endian */
    uint32_t packed;
    memcpy(&packed, s, sizeof packed);
    return (packed ^ 0x00746f68) == 0;
}

static inline uint64_t process_row(const char *row) {
    uint64_t id, a, b, c;
    const char *p = row + 1;           /* skip 'r' */
    id = parse_dec_u32(p, &p);
    ++p;                               /* skip comma */
    a  = parse_dec_u32(p, &p);
    ++p;
    b  = parse_dec_u32(p, &p);
    ++p;
    c  = parse_dec_u32(p, &p);
    ++p;
    uint64_t v = b + c + 14;
    if (match_hot(p)) v += 29;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t h0 = 1469598103934665603ULL;
    uint64_t h1 = 1469598103934665603ULL + prime;
    uint64_t h2 = 1469598103934665603ULL + 2*prime;
    uint64_t h3 = 1469598103934665603ULL + 3*prime;
    size_t i = 0;
    for (; i + 4 <= row_count; i += 4) {
        uint64_t v0 = process_row(rows[i+0]) + 1;
        uint64_t v1 = process_row(rows[i+1]) + 1;
        uint64_t v2 = process_row(rows[i+2]) + 1;
        uint64_t v3 = process_row(rows[i+3]) + 1;
        h0 ^= v0; h0 *= prime;
        h1 ^= v1; h1 *= prime;
        h2 ^= v2; h2 *= prime;
        h3 ^= v3; h3 *= prime;
    }
    /* tail */
    for (; i < row_count; ++i) {
        uint64_t v = process_row(rows[i]) + 1;
        h0 ^= v;
        h0 *= prime;
    }
    /* combine partial hashes */
    h0 ^= h1;
    h0 *= prime;
    h0 ^= h2;
    h0 *= prime;
    h0 ^= h3;
    h0 *= prime;
    return h0;
}