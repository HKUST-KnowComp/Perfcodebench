#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline int is_space(char c) {
    return c == ' ' || (c >= '\t' && c <= '\r');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // Check length 15
        if (r[0] != '\0' && r[1] != '\0' && r[2] != '\0' && r[3] != '\0' && 
            r[4] != '\0' && r[5] != '\0' && r[6] != '\0' && r[7] != '\0' && 
            r[8] != '\0' && r[9] != '\0' && r[10] != '\0' && r[11] != '\0' && 
            r[12] != '\0' && r[13] != '\0' && r[14] != '\0' && r[15] == '\0') {
            
            // sscanf(r, "%c%c%4u-%8X", &p0, &p1, &x, &y)
            // p0 = r[0], p1 = r[1]
            // %4u skips whitespace, then reads up to 4 digits
            int idx = 2;
            while (idx < 6 && is_space(r[idx])) idx++;
            
            int digits_x = 0;
            while (idx < 15 && digits_x < 4 && is_digit(r[idx])) {
                idx++;
                digits_x++;
            }

            if (digits_x > 0 && idx < 15 && r[idx] == '-') {
                idx++; // skip '-'
                while (idx < 15 && is_space(r[idx])) idx++;
                
                int digits_y = 0;
                while (idx < 15 && digits_y < 8 && is_hex(r[idx])) {
                    idx++;
                    digits_y++;
                }

                if (digits_y > 0) {
                    v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
