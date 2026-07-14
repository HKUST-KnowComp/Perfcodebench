#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Performance Engineer Optimization:
 * 1. Replaced sscanf with a manual character-level parser to avoid overhead.
 * 2. Replaced strlen with a fixed-length check (r[15] == '\0').
 * 3. Used a lookup table for fast validation of digits and hex characters.
 * 4. Minimized branching in the hot loop.
 */

static const uint8_t CHAR_MAP[256] = {
    ['0'] = 3, ['1'] = 3, ['2'] = 3, ['3'] = 3, ['4'] = 3, ['5'] = 3, ['6'] = 3, ['7'] = 3, ['8'] = 3, ['9'] = 3,
    ['A'] = 2, ['B'] = 2, ['C'] = 2, ['D'] = 2, ['E'] = 2, ['F'] = 2,
    ['a'] = 2, ['b'] = 2, ['c'] = 2, ['d'] = 2, ['e'] = 2, ['f'] = 2
};

// Bit 0: is hex digit, Bit 1: is decimal digit
#define IS_HEX(c) (CHAR_MAP[(unsigned char)(c)] & 1)
#define IS_DIGIT(c) (CHAR_MAP[(unsigned char)(c)] & 2)

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // Check for exact length 15 and basic format structure
        // Format: %c%c %4u - %8X
        // Indices: 01 2345 6 78901234
        if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && r[6] && r[7] && 
            r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && r[15] == '\0') {
            
            // sscanf("%4u") requires at least one digit at r[2]
            if (IS_DIGIT(r[2])) {
                int k = 1;
                if (IS_DIGIT(r[3])) { k++; if (IS_DIGIT(r[4])) { k++; if (IS_DIGIT(r[5])) { k++; } } }
                
                // After %4u, we must find a '-'
                if (r[2 + k] == '-') {
                    // After '-', sscanf("%8X") requires at least one hex digit
                    if (IS_HEX(r[2 + k + 1])) {
                        // If all conditions met, calculate v
                        v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                    }
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
