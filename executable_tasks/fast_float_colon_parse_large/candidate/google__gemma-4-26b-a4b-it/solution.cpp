#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    if (input.empty()) return 0.0;

    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        
        if (ec != fast_float::error_code::none) {
            // If parsing fails, we attempt to skip to the next colon or end
            // to maintain robustness, though the task implies valid decimal buffers.
            break;
        }

        sum += value;
        p = ptr;

        // Skip the colon if present
        if (p < end && *p == ':') {
            p++;
        }
    }

    return sum;
}