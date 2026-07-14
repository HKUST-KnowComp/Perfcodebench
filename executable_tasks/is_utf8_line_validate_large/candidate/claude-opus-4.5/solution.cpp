#include "interface.h"
#include <is_utf8.h>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    const char* data = input.data();
    const char* end = data + input.size();
    std::size_t count = 0;
    
    while (data < end) {
        const char* nl = static_cast<const char*>(std::memchr(data, '\n', end - data));
        const char* line_end = nl ? nl : end;
        std::size_t len = line_end - data;
        
        if (is_utf8(data, len)) {
            ++count;
        }
        
        data = line_end + 1;
    }
    
    return count;
}