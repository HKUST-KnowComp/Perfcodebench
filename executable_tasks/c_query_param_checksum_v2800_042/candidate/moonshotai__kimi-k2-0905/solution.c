#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* p = (const unsigned char*)rows[i];
        /* --- scan u=val --- */
        while (*p != '=') ++p; ++p; /* skip past "u=" */
        int u = 0;
        while (*p >= '0' && *p <= '9') { u = u * 10 + (*p - '0'); ++p; }
        /* --- scan s=val --- */
        while (*p != '=') ++p; ++p; /* skip past "&s=" */
        int s = 0;
        while (*p >= '0' && *p <= '9') { s = s * 10 + (*p - '0'); ++p; }
        /* --- scan f=val --- */
        while (*p != '=') ++p; ++p; /* skip past "&f=" */
        int f = 0;
        while (*p >= '0' && *p <= '9') { f = f * 10 + (*p - '0'); ++p; }
        /* --- combine value and mix into hash --- */
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}