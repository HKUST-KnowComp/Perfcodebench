#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];
        /* load 16 bytes (safe: rows are 0-terminated and at least 15 chars) */
        uint64_t v;
        memcpy(&v, s, 8);
        uint64_t v2;
        memcpy(&v2, s + 8, 8);

        /* check fixed positions: dash at 6, two hex digits at 11-12 */
        uint64_t dash = 0x00000000002D0000ULL;          /* dash at byte 6 */
        uint64_t hex  = 0x000000000000FF00ULL;          /* byte 11,12 mask */
        uint64_t dash_ok = ((v >> (6 * 8)) & 0xFF) == 0x2D;
        uint64_t h1 = (v2 >> (3 * 8)) & 0xFF;
        uint64_t h2 = (v2 >> (2 * 8)) & 0xFF;
        uint64_t hex_ok = ((h1 | h2) & 0x80) == 0;      /* both < 0x80 */
        uint64_t ok = dash_ok & hex_ok;

        /* extract wanted bytes: s[2] and s[7] */
        uint64_t wanted = (uint64_t)s[2] + (uint64_t)s[7];
        uint64_t add = (wanted + 1ULL) & -(int64_t)ok;

        hash ^= add;
        hash *= prime;
    }
    return hash;
}