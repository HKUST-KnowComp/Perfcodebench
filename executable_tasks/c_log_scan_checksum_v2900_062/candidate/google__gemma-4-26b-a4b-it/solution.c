#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The format is %c%c%4u-%8X (total 15 chars)
        // Example: AA1234-ABCDEFGH (Wait, the format string says %4u-%8X)
        // Let's look at the original logic: 
        // matched == 4 && strlen(rows[i]) == 15u
        // v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
        
        // To preserve correctness, we must ensure the string is exactly 15 chars
        // and matches the pattern: char, char, digit, digit, digit, digit, '-', hex...
        // However, the original code only checks strlen == 15 and sscanf == 4.
        // sscanf(rows[i], "%c%c%4u-%8X", ...) 
        // %c (1) + %c (1) + %4u (4) + '-' (1) + %8X (8) = 15 chars.

        // Fast length check and pattern validation
        // We check if s[14] is the last char (null terminator check is implicit in length)
        // and if s[6] is '-'. We also need to ensure the 4u and 8X parts are valid.
        
        // Optimization: Instead of full sscanf, we check the fixed positions.
        // The original code's 'v' only depends on rows[i][2] and rows[i][7].
        
        // We must verify the string is exactly 15 chars and matches the sscanf pattern.
        // A string of length 15 has indices 0-14. s[15] must be '\0'.
        if (s[0] != '\0' && s[14] != '\0' && s[15] == '\0') {
            // Check '-' at index 6
            if (s[6] == '-') {
                // Check if the 4u part (indices 2,3,4,5) are digits
                if (s[2] >= '0' && s[2] <= '9' &&
                    s[3] >= '0' && s[3] <= '9' &&
                    s[4] >= '0' && s[4] <= '9' &&
                    s[5] >= '0' && s[5] <= '9') {
                    
                    // Check if the 8X part (indices 7-14) are hex
                    // We can do this via a small loop or unrolled
                    int hex_ok = 1;
                    for (int j = 7; j <= 14; ++j) {
                        char c = s[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            hex_ok = 0;
                            break;
                        }
                    }

                    if (hex_ok) {
                        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                    }
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}