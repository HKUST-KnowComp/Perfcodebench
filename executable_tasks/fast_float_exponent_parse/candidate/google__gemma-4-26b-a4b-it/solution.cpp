#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading whitespace/newlines if any exist between numbers
        while (p < end && (*p == '\n' || *p == '\r' || *p == ' ')) {
            p++;
        }
        if (p >= end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
        } else {
            // If parsing fails, we attempt to skip to the next line to maintain robustness
            // though per task description, we expect valid scientific notation.
            while (p < end && *p != '\n') {
                p++;
            }
            if (p < end) p++;
        }
    }
    return sum;
}