#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        // skip 'r'
        p++;
        // skip id digits
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip ',' after id
        // skip a digits
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip ',' after a
        // parse b
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++; // skip ',' after b
        // parse c
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++; // skip ',' after c
        // check flag
        uint64_t v = (uint64_t)(b + c + 16);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}