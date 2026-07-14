#include "interface.h"
#include "fast_float/fast_float.h"
#include <vector>
#include <algorithm>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    const char* p = input.data();
    const char* end = p + input.size();
    
    // Buffer to handle 'd' to 'e' conversion without heap allocation per line
    static thread_local std::vector<char> buffer;
    
    while (p < end) {
        const char* line_end = static_cast<const char*>(memchr(p, '\n', end - p));
        if (!line_end) line_end = end;
        
        size_t len = line_end - p;
        if (len > 0) {
            buffer.assign(p, line_end);
            for (size_t i = 0; i < len; ++i) {
                if (buffer[i] == 'd' || buffer[i] == 'D') {
                    buffer[i] = 'e';
                }
            }
            
            double val;
            auto result = fast_float::from_chars(buffer.data(), buffer.data() + len, val);
            if (result.ec == std::errc()) {
                sum += val;
            }
        }
        
        p = line_end + 1;
    }
    return sum;
}