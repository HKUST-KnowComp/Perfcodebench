#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const size_t len = text.size();
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const char* start = data;
        const char* end_ptr = data + len;
        
        while (start <= end_ptr) {
            const char* nl = static_cast<const char*>(memchr(start, '\n', end_ptr - start));
            const char* line_end = nl ? nl : end_ptr;
            size_t line_len = line_end - start;
            
            // mix line length
            hash ^= line_len;
            hash *= 1099511628211ULL;
            
            // mix first char (or 0 if empty)
            uint64_t first_char = (line_len > 0) ? static_cast<unsigned char>(*start) : 0u;
            hash ^= first_char;
            hash *= 1099511628211ULL;
            
            if (!nl) break;
            start = nl + 1;
        }
    }
    return hash;
}