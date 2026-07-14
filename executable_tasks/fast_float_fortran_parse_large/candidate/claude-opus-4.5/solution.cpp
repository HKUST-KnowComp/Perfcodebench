#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstring>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();
    double sum = 0.0;
    
    while (p < end) {
        // Skip leading whitespace and newlines
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
            ++p;
        }
        if (p >= end) break;
        
        double value;
        auto result = fast_float::from_chars_advanced(
            p, end, value,
            fast_float::parse_options{fast_float::chars_format::fortran}
        );
        
        if (result.ec == std::errc{}) {
            sum += value;
            p = result.ptr;
        } else {
            // Skip to next line on parse error
            while (p < end && *p != '\n') ++p;
            if (p < end) ++p;
        }
    }
    
    return sum;
}
