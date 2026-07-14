#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];
        /* Fixed format: ccNNNN-NNNNNNNN (15 chars)
           We need chars 2 and 7, plus validate the format:
           positions 0,1,3-6,8-14 must be hex digits, '-' at pos 2 & 7.
           Only if all are satisfied do we add (s[2] + s[7]) to the hash. */
        uint64_t v = 0;
        if ((s[2] == '-') && (s[7] == '-') &&
            (s[15] == 0) &&                 /* exact length 15 */
            ((uint8_t)(s[0]  - '0') <= 9 || ((uint8_t)(s[0]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[1]  - '0') <= 9 || ((uint8_t)(s[1]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[3]  - '0') <= 9 || ((uint8_t)(s[3]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[4]  - '0') <= 9 || ((uint8_t)(s[4]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[5]  - '0') <= 9 || ((uint8_t)(s[5]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[6]  - '0') <= 9 || ((uint8_t)(s[6]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[8]  - '0') <= 9 || ((uint8_t)(s[8]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[9]  - '0') <= 9 || ((uint8_t)(s[9]  | 32) - 'a') <= 5) &&
            ((uint8_t)(s[10] - '0') <= 9 || ((uint8_t)(s[10] | 32) - 'a') <= 5) &&
            ((uint8_t)(s[11] - '0') <= 9 || ((uint8_t)(s[11] | 32) - 'a') <= 5) &&
            ((uint8_t)(s[12] - '0') <= 9 || ((uint8_t)(s[12] | 32) - 'a') <= 5) &&
            ((uint8_t)(s[13] - '0') <= 9 || ((uint8_t)(s[13] | 32) - 'a') <= 5) &&
            ((uint8_t)(s[14] - '0') <= 9 || ((uint8_t)(s[14] | 32) - 'a') <= 5)) {
            v = (uint64_t)((uint8_t)s[2] + (uint8_t)s[7]);
        }
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}