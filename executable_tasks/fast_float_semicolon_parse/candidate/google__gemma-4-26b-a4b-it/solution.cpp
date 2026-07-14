#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    if (input.empty()) {
        return 0.0;
    }

    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec != fast_float::error_code::no_conversion) {
            // If no conversion was possible, we might be at a semicolon or end of string
            // In a well-formed semicolon-delimited list, we skip to the next delimiter
            if (p < end && *p == ';') {
                p++;
                continue;
            }
            break;
        }

        sum += value;
        p = ptr;

        // Skip the semicolon if present
        if (p < end && *p == ';') {
            p++;
        }
    }

    return sum;
}