#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* p = input.data();
    const char* const end = p + input.size();
    
    while (p < end) {
        // Use memchr for fast newline search (typically SIMD-optimized in libc)
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end - p)));
        const char* line_end = nl ? nl : end;
        std::size_t len = static_cast<std::size_t>(line_end - p);
        
        // Use the optimized is_utf8 library for validation
        if (is_utf8(p, len)) {
            ++count;
        }
        
        p = (line_end < end) ? (line_end + 1) : end;
    }
    
    return count;
}