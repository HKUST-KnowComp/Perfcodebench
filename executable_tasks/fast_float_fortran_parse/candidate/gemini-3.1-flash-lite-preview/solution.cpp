#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <vector>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    char buffer[64];

    while (p < end) {
        const char* line_start = p;
        while (p < end && *p != '\n') {
            p++;
        }
        
        size_t len = p - line_start;
        if (len > 0) {
            size_t copy_len = (len < 63) ? len : 63;
            for (size_t i = 0; i < copy_len; ++i) {
                char c = line_start[i];
                if (c == 'd' || c == 'D') {
                    buffer[i] = 'e';
                } else {
                    buffer[i] = c;
                }
            }
            
            double val;
            auto result = fast_float::from_chars(buffer, buffer + copy_len, val);
            if (result.ec == std::errc()) {
                sum += val;
            }
        }
        
        if (p < end && *p == '\n') {
            p++;
        }
    }
    return sum;
}