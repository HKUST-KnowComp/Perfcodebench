#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* ptr = rows[i];
        ptr++; // Skip leading 'r' character
        
        // Parse ID field
        int id = 0;
        while (*ptr != ',') {
            id = id * 10 + (*ptr - '0');
            ptr++;
        }
        ptr++;
        
        // Parse 'a' field
        int a = 0;
        while (*ptr != ',') {
            a = a * 10 + (*ptr - '0');
            ptr++;
        }
        ptr++;
        
        // Parse 'b' field
        int b = 0;
        while (*ptr != ',') {
            b = b * 10 + (*ptr - '0');
            ptr++;
        }
        ptr++;
        
        // Parse 'c' field
        int c = 0;
        while (*ptr != ',') {
            c = c * 10 + (*ptr - '0');
            ptr++;
        }
        ptr++;
        
        // Exact check for "hot" flag, equivalent to strcmp(flag, "hot") == 0
        int is_hot = (ptr[0] == 'h' && ptr[1] == 'o' && ptr[2] == 't' && ptr[3] == '\0');
        
        uint64_t v = (uint64_t)(b + c + 22);
        if (is_hot) v += 29ULL;
        
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}