#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const size_t len = text.size();
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const char* ptr = data;
        const char* end = data + len;
        
        while (ptr < end) {
            const char* nl = static_cast<const char*>(memchr(ptr, '\n', end - ptr));
            const char* line_end = nl ? nl : end;
            size_t line_len = line_end - ptr;
            
            // mix line length
            hash ^= line_len;
            hash *= 1099511628211ULL;
            
            // mix first char (or 0 if empty)
            uint64_t first_char = (line_len > 0) ? static_cast<unsigned char>(*ptr) : 0u;
            hash ^= first_char;
            hash *= 1099511628211ULL;
            
            ptr = line_end + 1;
        }
    }
    return hash;
}