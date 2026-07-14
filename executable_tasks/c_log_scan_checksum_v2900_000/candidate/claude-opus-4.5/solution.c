#include "interface.h"

#include <stdint.h>
#include <string.h>

static const uint8_t is_digit[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,['5']=1,['6']=1,['7']=1,['8']=1,['9']=1
};

static const uint8_t is_hex[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,['5']=1,['6']=1,['7']=1,['8']=1,['9']=1,
    ['A']=1,['B']=1,['C']=1,['D']=1,['E']=1,['F']=1,
    ['a']=1,['b']=1,['c']=1,['d']=1,['e']=1,['f']=1
};

static inline int validate_row(const char* row) {
    const unsigned char* r = (const unsigned char*)row;
    
    // Check digits at positions 2,3,4,5
    if (!is_digit[r[2]] || !is_digit[r[3]] || !is_digit[r[4]] || !is_digit[r[5]])
        return 0;
    
    // Check dash at position 6
    if (r[6] != '-')
        return 0;
    
    // Check hex digits at positions 7-14
    if (!is_hex[r[7]] || !is_hex[r[8]] || !is_hex[r[9]] || !is_hex[r[10]] ||
        !is_hex[r[11]] || !is_hex[r[12]] || !is_hex[r[13]] || !is_hex[r[14]])
        return 0;
    
    // Check null terminator at position 15
    if (r[15] != '\0')
        return 0;
    
    return 1;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    size_t i = 0;
    
    // Process 4 rows at a time
    for (; i + 4 <= row_count; i += 4) {
        const unsigned char* r0 = (const unsigned char*)rows[i];
        const unsigned char* r1 = (const unsigned char*)rows[i+1];
        const unsigned char* r2 = (const unsigned char*)rows[i+2];
        const unsigned char* r3 = (const unsigned char*)rows[i+3];
        
        uint64_t v0 = 0, v1 = 0, v2 = 0, v3 = 0;
        
        if (validate_row((const char*)r0))
            v0 = (uint64_t)(r0[2] + r0[7]);
        
        if (validate_row((const char*)r1))
            v1 = (uint64_t)(r1[2] + r1[7]);
        
        if (validate_row((const char*)r2))
            v2 = (uint64_t)(r2[2] + r2[7]);
        
        if (validate_row((const char*)r3))
            v3 = (uint64_t)(r3[2] + r3[7]);
        
        hash ^= (v0 + 1ULL);
        hash *= prime;
        hash ^= (v1 + 1ULL);
        hash *= prime;
        hash ^= (v2 + 1ULL);
        hash *= prime;
        hash ^= (v3 + 1ULL);
        hash *= prime;
    }
    
    // Handle remaining rows
    for (; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        
        if (validate_row((const char*)r))
            v = (uint64_t)(r[2] + r[7]);
        
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    
    return hash;
}
