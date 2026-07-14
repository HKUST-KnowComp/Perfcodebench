#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char*)rows[i];
        /* Fast length check: length must be 15 */
        if (s[15] != '\0') continue;
        /* Fast pattern check: positions 2 and 7 must be '-' and ' ' respectively */
        if (s[2] != '-' || s[7] != ' ') continue;
        /* Extract two digits from s[3..6] and s[8..15] without full parsing */
        uint32_t part1 = (uint32_t)(s[3]-'0')*1000 + (uint32_t)(s[4]-'0')*100 + (uint32_t)(s[5]-'0')*10 + (uint32_t)(s[6]-'0');
        uint32_t part2 = (uint32_t)(s[8]-'0')*10000000 + (uint32_t)(s[9]-'0')*1000000 + (uint32_t)(s[10]-'0')*100000 + (uint32_t)(s[11]-'0')*10000 + (uint32_t)(s[12]-'0')*1000 + (uint32_t)(s[13]-'0')*100 + (uint32_t)(s[14]-'0')*10 + (uint32_t)(s[15]-'0');
        /* Build value to hash: part1 + part2 */
        uint64_t v = (uint64_t)part1 + (uint64_t)part2;
        /* Update FNV-1a hash */
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}