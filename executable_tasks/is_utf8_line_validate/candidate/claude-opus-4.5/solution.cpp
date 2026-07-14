#include "interface.h"
#include <is_utf8.h>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* data = input.data();
    const char* end = data + input.size();
    const char* line_start = data;
    
    while (line_start < end) {
        const char* newline = static_cast<const char*>(std::memchr(line_start, '\n', end - line_start));
        const char* line_end = newline ? newline : end;
        std::size_t line_len = line_end - line_start;
        
        if (is_utf8(line_start, line_len)) {
            ++count;
        }
        
        if (newline) {
            line_start = newline + 1;
        } else {
            break;
        }
    }
    
    return count;
}