#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip whitespace
        while (p < end && (*p == ' ' || (*p >= '\t' && *p <= '\r')))
            ++p;

        if (p >= end) break;

        // Use fast_float for high-performance parsing
        double value;
        const auto result = fast_float::from_chars(p, end, value);
        
        if (result.ptr == p) {
            // Handle cases where no number was found at current position
            ++p;
            continue;
        }

        sum += value;
        p = result.ptr;
    }
    return sum;
}