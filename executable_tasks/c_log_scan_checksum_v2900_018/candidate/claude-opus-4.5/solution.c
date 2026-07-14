#include "interface.h"

#include <stdint.h>
#include <string.h>

static const unsigned char is_digit[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,['5']=1,['6']=1,['7']=1,['8']=1,['9']=1
};

static const unsigned char is_hex[256] = {
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,['5']=1,['6']=1,['7']=1,['8']=1,['9']=1,
    ['A']=1,['B']=1,['C']=1,['D']=1,['E']=1,['F']=1,
    ['a']=1,['b']=1,['c']=1,['d']=1,['e']=1,['f']=1
};

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        
        /* Format: CC9999-HHHHHHHH (15 chars)
         * positions: 0,1 = any char (p0,p1)
         *            2,3,4,5 = digits
         *            6 = '-'
         *            7-14 = hex digits
         */
        
        /* Check length is exactly 15 */
        if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && r[6] && 
            r[7] && r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && !r[15]) {
            
            /* Validate format */
            if (is_digit[r[2]] && is_digit[r[3]] && is_digit[r[4]] && is_digit[r[5]] &&
                r[6] == '-' &&
                is_hex[r[7]] && is_hex[r[8]] && is_hex[r[9]] && is_hex[r[10]] &&
                is_hex[r[11]] && is_hex[r[12]] && is_hex[r[13]] && is_hex[r[14]]) {
                
                v = (uint64_t)(r[2] + r[7]);
            }
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}