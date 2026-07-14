#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    if (input.empty()) return 0.0;

    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading semicolons or whitespace if any
        while (p < end && (*p == ';' || *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
            p++;
        }
        if (p >= end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
            // Advance past the delimiter if present
            if (p < end && *p == ';') {
                p++;
            }
        } else {
            // If parsing fails, move forward to find next potential number
            // to prevent infinite loops in malformed input
            while (p < end && *p != ';') {
                p++;
            }
            if (p < end && *p == ';') p++;
        }
    }

    return sum;
}