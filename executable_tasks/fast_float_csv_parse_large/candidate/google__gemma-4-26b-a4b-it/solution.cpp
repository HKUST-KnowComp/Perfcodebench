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
        // Skip leading commas if any (though CSV usually has them between values)
        if (*p == ',') {
            p++;
            continue;
        }

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error::invalid_argument || ptr == p) {
            // If we can't parse a number, we attempt to skip to the next comma to stay robust
            // or break if it's the end of the string.
            while (p < end && *p != ',') {
                p++;
            }
            if (p < end && *p == ',') p++;
            continue;
        }

        sum += value;
        p = ptr;

        // Advance past the comma if present
        if (p < end && *p == ',') {
            p++;
        }
    }

    return sum;
}