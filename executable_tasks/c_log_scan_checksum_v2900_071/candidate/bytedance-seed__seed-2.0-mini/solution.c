#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        uint64_t v = 0;
        const char* s = rows[i];
        if (strlen(s) == 15) {
            if (s[6] == '-') {
                char c;
                // Check positions 2-5 are digits
                c = s[2];
                if (c < '0' || c > '9') goto invalid;
                c = s[3];
                if (c < '0' || c > '9') goto invalid;
                c = s[4];
                if (c < '0' || c > '9') goto invalid;
                c = s[5];
                if (c < '0' || c > '9') goto invalid;

                // Check positions 7-14 are hex digits
                c = s[7];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[8];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[9];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[10];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[11];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[12];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[13];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;
                c = s[14];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) goto invalid;

                v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
            }
        }
    invalid:
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}