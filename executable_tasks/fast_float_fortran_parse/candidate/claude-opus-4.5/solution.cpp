#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstring>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();
    
    double sum = 0.0;
    double c = 0.0;  // Kahan summation compensation
    
    while (p < end) {
        // Skip leading whitespace and empty lines
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
            ++p;
        }
        if (p >= end) break;
        
        // Find end of current number (next whitespace or newline)
        const char* num_start = p;
        
        double value;
        auto result = fast_float::from_chars_advanced(
            num_start, end, value,
            fast_float::chars_format::fortran
        );
        
        if (result.ec == std::errc() && result.ptr != num_start) {
            // Kahan summation for better precision
            double y = value - c;
            double t = sum + y;
            c = (t - sum) - y;
            sum = t;
            p = result.ptr;
        } else {
            // Skip to next line on parse failure
            while (p < end && *p != '\n') ++p;
            if (p < end) ++p;
        }
    }
    
    return sum;
}