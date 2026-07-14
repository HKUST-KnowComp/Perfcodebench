#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value = 0.0;
        const char* next = p;
        
        // fast_float::parse_double is highly optimized for scientific notation
        auto result = fast_float::parse_double(p, end, value);
        
        if (result.ec != std::errc{}) {
            break;
        }
        
        sum += value;
        p = result.ptr;

        // Skip tab delimiter
        if (p < end && *p == '\t') {
            p++;
        }
    }
    return sum;
}