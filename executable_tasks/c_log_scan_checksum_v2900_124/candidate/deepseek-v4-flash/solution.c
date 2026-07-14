#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        // manual length check (up to 16)
        int len = 0;
        while (len < 16 && row[len]) ++len;
        if (len != 15) {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // check pattern: row[0] row[1] row[2-5] digits, row[6] '-', row[7-14] hex
        if (row[6] != '-' ||
            row[2] < '0' || row[2] > '9' ||
            row[3] < '0' || row[3] > '9' ||
            row[4] < '0' || row[4] > '9' ||
            row[5] < '0' || row[5] > '9')
        {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // check hex digits row[7..14]
        int hex_ok = 1;
        for (int j = 7; j <= 14; ++j) {
            unsigned char c = (unsigned char)row[j];
            if (!((c >= '0' && c <= '9') ||
                  (c >= 'a' && c <= 'f') ||
                  (c >= 'A' && c <= 'F'))) {
                hex_ok = 0;
                break;
            }
        }
        if (!hex_ok) {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        uint64_t v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
