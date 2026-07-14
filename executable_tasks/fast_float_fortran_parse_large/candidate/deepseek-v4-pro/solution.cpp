#include "interface.h"

#include <cstring>
#include <fast_float/fast_float.h>
#include <string>
#include <system_error>

double parse_sum(const std::string& input) {
    if (input.empty()) return 0.0;
    
    // Work on a mutable copy to replace Fortran exponent markers
    std::string buffer = input;
    for (char& c : buffer) {
        if (c == 'd') c = 'e';
        else if (c == 'D') c = 'E';
    }
    
    const char* p = buffer.data();
    const char* end = p + buffer.size();
    double sum = 0.0;
    
    while (p < end) {
        // Find next newline or end of buffer
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        if (!line_end) {
            line_end = end;
        }
        
        // Parse number from the line if not empty
        if (p < line_end) {
            double value = 0.0;
            auto result = fast_float::from_chars(p, line_end, value, fast_float::chars_format::general);
            if (result.ec == std::errc()) {
                sum += value;
            }
        }
        
        // Advance past line (skip newline if present)
        if (line_end < end) {
            p = line_end + 1;
        } else {
            break;
        }
    }
    
    return sum;
}
