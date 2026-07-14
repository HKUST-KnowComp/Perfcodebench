#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        /* fixed-format is always 15 bytes; positions of interest are 2 and 7 */
        hash ^= (uint64_t)(r[2] + r[7] + 1);
        hash *= 1099511628211ULL;
    }
    return hash;
}