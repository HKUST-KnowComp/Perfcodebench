#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;
        
        /* 
         * The original logic requires: 
         * 1. sscanf matched 4 items (%c%c%4u-%8X)
         * 2. strlen(rows[i]) == 15
         * 
         * Given the format: CCUU-HHHHHHHH (where C=char, U=uint, H=hex)
         * Total length: 2 (chars) + 4 (uint) + 1 (dash) + 8 (hex) = 15.
         * 
         * To preserve correctness, we must ensure the string is exactly 15 chars
         * and follows the pattern. However, the checksum only uses indices 2 and 7.
         * Index 2 is the first digit of the 4u. Index 7 is the first digit of the 8X.
         * 
         * We must verify the '-' is at index 6 and the string is 15 chars long.
         * We also check for null terminator at index 15 to satisfy strlen == 15.
         */
        
        if (r[6] == '-' && r[15] == '\0') {
            /* 
             * sscanf validation for %4u and %8X is complex to replicate perfectly 
             * without overhead, but the checksum only depends on specific bytes.
             * The original code: v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
             * We check if the characters at the expected positions are valid for the types.
             */
            
            // Check if index 2 is a digit (part of %4u)
            // Check if index 7 is a hex digit (part of %8X)
            // Note: sscanf returns 4 if all match. 
            // For the checksum to be calculated, the string must be valid.
            
            // Optimization: The original code's 'matched == 4' is the bottleneck.
            // We perform a lightweight check for the required characters.
            
            // Check if r[2..5] are digits and r[7..14] are hex
            // This is a heuristic to mimic sscanf's success for the specific checksum indices.
            // However, to be strictly correct to the original's behavior:
            
            int valid = 1;
            // Check digits for %4u (indices 2,3,4,5)
            for(int j=2; j<=5; ++j) if(r[j] < '0' || r[j] > '9') { valid = 0; break; }
            if(valid) {
                // Check hex for %8X (indices 7,8,9,10,11,12,13,14)
                for(int j=7; j<=14; ++j) {
                    char c = r[j];
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                        valid = 0; break;
                    }
                }
            }

            if (valid) {
                v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}