#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Helper to check if a character is a hex digit (0-9, A-F)
static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

// Helper to check if a character is a decimal digit
static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // The format is: %c%c%4u-%8X
        // Total length must be 15: 2 (chars) + 4 (digits) + 1 (dash) + 8 (hex) = 15
        // We check the structure manually to replace sscanf and strlen.
        
        // Check length and basic structure via character validation
        // Indices: 0,1 (chars), 2,3,4,5 (digits), 6 (dash), 7,8,9,10,11,12,13,14 (hex)
        // We must also ensure the string is exactly 15 chars (no null terminator at 15)
        
        if (r[15] == '\0' &&
            is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
            r[6] == '-' &&
            is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
            is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14])) {
            
            // The original logic: v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7]);
            // Note: sscanf matched 4 items. The 3rd was %4u (digits at 2,3,4,5)
            // and the 4th was %8X (hex at 7,8,9,10,11,12,13,14).
            // The condition 'matched == 4' is satisfied if the pattern holds.
            v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}