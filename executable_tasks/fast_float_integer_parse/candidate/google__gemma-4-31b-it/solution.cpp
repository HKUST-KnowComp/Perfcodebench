#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading whitespace/newlines to find the start of a token
        while (p < end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) {
            p++;
        }
        if (p >= end) break;

        double value;
        const char* next;
        // fast_float::from_chars is significantly faster than strtod
        if (fast_float::from_chars(p, end, value, &next)) {
            sum += value;
            p = next;
        } else {
            // In case of a parsing error, move forward to avoid infinite loop
            p++;
        }
    }
    return sum;
}