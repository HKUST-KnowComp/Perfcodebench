#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading whitespace/newlines if any
        while (p < end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) {
            p++;
        }
        if (p >= end) break;

        double value;
        const auto result = fast_float::from_chars(p, end, value);
        
        if (result.ptr == p) {
            // Parsing failed or no number found
            break;
        }

        sum += value;
        p = result.ptr;
    }
    return sum;
}