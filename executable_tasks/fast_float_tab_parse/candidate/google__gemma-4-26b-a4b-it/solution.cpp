#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading whitespace or tabs
        while (p < end && (*p == '\t' || *p == ' ' || *p == '\n' || *p == '\r')) {
            p++;
        }
        
        if (p >= end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
        } else {
            // If parsing fails, move forward to avoid infinite loop
            // In a well-formed tab-delimited file, this shouldn't happen
            p++;
        }
    }

    return sum;
}