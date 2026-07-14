#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The format is %c%c%4u-%8X (length 15)
        // Indices: 01 2345 6 789ABCDE
        // Example: AB1234-ABCDEFGH (Wait, the format says %4u-%8X)
        // Let's re-examine the baseline logic:
        // sscanf(rows[i], "%%c%%c%%4u-"%%8X", &p0, &p1, &x, &y);
        // strlen(rows[i]) == 15
        // v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);

        // To match correctness, we must ensure sscanf would have succeeded.
        // 1. Length must be 15.
        // 2. sscanf needs 2 chars, 4 digits, a dash, and 8 hex chars.
        // Total: 2 + 4 + 1 + 8 = 15.

        // Check length and basic structure via manual scan to avoid sscanf overhead
        // We check if the string is exactly 15 chars and matches the pattern
        // However, the baseline only calculates v if matched == 4 AND strlen == 15.
        
        // Fast length check and pattern validation
        // We need to verify: s[0], s[1] are chars, s[2..5] are digits, s[6] is '-', s[7..14] are hex.
        // But the baseline only uses s[2] and s[7] for the checksum.
        
        // Let's implement a high-speed validation of the sscanf requirements.
        int valid = 1;
        // Check length 15 (null terminator at index 15)
        // We check indices 0-14. If s[15] is not '\0', it's not length 15.
        // But we can't easily check s[15] without knowing if it's safe. 
        // However, the input is a const char* from a harness, usually null-terminated.
        
        // Optimization: The baseline's 'v' only depends on s[2] and s[7].
        // We must ensure sscanf would return 4.
        // sscanf(rows[i], "%c%c%4u-%8X", &p0, &p1, &x, &y)
        // This requires: s[0], s[1] exist; s[2..5] are digits; s[6] is '-'; s[7..14] are hex.
        
        // Check length 15 first
        // We use a manual loop or unrolled check for the 15 chars.
        // To be safe and fast, we check the specific characters required for sscanf success.
        
        // Check s[6] == '-'
        if (s[6] == '-') {
            // Check s[2..5] are digits
            if (s[2] >= '0' && s[2] <= '9' &&
                s[3] >= '0' && s[3] <= '9' &&
                s[4] >= '0' && s[4] <= '9' &&
                s[5] >= '0' && s[5] <= '9') {
                
                // Check s[7..14] are hex
                // And check s[15] is '\0' to satisfy strlen == 15
                int hex_ok = 1;
                for (int j = 7; j < 15; ++j) {
                    char c = s[j];
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                        hex_ok = 0;
                        break;
                    }
                }
                
                if (hex_ok && s[15] == '\0') {
                    v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}