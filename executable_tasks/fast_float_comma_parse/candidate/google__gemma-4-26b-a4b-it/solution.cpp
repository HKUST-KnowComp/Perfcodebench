#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    if (input.empty()) return 0.0;

    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading commas if any
        if (*p == ',') {
            p++;
            continue;
        }

        double value = 0.0;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
            // If we stopped at a comma, skip it for the next iteration
            if (p < end && *p == ',') {
                p++;
            }
        } else {
            // If parsing failed, move forward to find next potential number
            // or break to prevent infinite loop
            while (p < end && *p != ',') {
                p++;
            }
            if (p < end && *p == ',') {
                p++;
            }
        }
    }

    return sum;
}