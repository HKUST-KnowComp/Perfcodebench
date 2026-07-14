#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t FNV_prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* load 16 bytes (row is 15 bytes + '\0') */
        uint64_t lo, hi;
        memcpy(&lo, s, 8);
        memcpy(&hi, s + 8, 8);

        /* build 64-bit mask of expected fixed characters */
        const uint64_t dash_bit   = 0x000000000000FF00ULL;  /* pos 4 */
        const uint64_t dash_pat   = 0x0000000000002D00ULL;  /* '-' << 8 */
        const uint64_t mask       = 0x000000000000FF00ULL;
        const uint64_t expected   = dash_pat;

        uint64_t v = lo & mask;
        uint64_t ok = (v == expected) & ((lo >> 32) == 0);  /* upper 4 bytes zero => len <= 15 */

        /* extract bytes at indexes 2 and 7 */
        uint64_t byte2 = (lo >> 16) & 0xFF;
        uint64_t byte7 = (lo >> 56) | ((hi & 0xFF) << 8);  /* hi[0] */
        uint64_t sum = byte2 + byte7;

        /* fold condition into value: 0 if not matched */
        uint64_t add = (sum + 1) & -(uint64_t)ok;

        hash ^= add;
        hash *= FNV_prime;
    }
    return hash;
}