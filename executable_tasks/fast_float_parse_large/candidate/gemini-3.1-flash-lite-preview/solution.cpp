#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    
    while (p < end) {
        // Skip potential whitespace or empty lines if necessary
        if (*p == '\n' || *p == '\r' || *p == ' ') {
            p++;
            continue;
        }

        double value;
        auto result = fast_float::from_chars(p, end, value);
        
        if (result.ec != std::errc()) {
            break;
        }
        
        sum += value;
        p = result.ptr;
        
        // Advance past the delimiter
        if (p < end && (*p == '\n' || *p == '\r')) {
            p++;
        }
    }
    return sum;
}