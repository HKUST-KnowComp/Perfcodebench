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
        double value = 0.0;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
            // Skip the comma if present
            if (p < end && *p == ',') {
                p++;
            }
        } else {
            // If parsing fails (e.g., non-numeric data), skip to next comma or end
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