#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip leading commas or whitespace
        while (p < end && (*p == ',' || *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) {
            p++;
        }
        
        if (p >= end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error::invalid_argument || ec == fast_float::error::result_out_of_range) {
            // If parsing fails, we attempt to skip this segment to maintain robustness
            // though per task constraints, we assume valid scientific notation.
            while (p < end && *p != ',') p++;
            if (p < end) p++;
            continue;
        }

        sum += value;
        p = ptr;
    }

    return sum;
}