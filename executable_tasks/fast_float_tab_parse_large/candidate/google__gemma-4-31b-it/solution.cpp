#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        const auto result = fast_float::from_chars(p, end, value);
        
        if (result.ptr == p) {
            // No number found at current position, skip potential whitespace/tabs
            if (*p == '\t') {
                p++;
                continue;
            } else {
                break;
            }
        }

        sum += value;
        p = result.ptr;

        // Skip the tab delimiter if present
        if (p < end && *p == '\t') {
            p++;
        }
    }
    return sum;
}