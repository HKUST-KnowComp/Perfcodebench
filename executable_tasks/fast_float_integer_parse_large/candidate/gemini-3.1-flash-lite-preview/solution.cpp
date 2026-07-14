#include "interface.h"
#include "fast_float/fast_float.h"
#include <string_view>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    std::string_view sv(input);
    const char* p = sv.data();
    const char* end = p + sv.size();

    while (p < end) {
        double value;
        auto result = fast_float::from_chars(p, end, value);
        if (result.ec != std::errc()) {
            // Skip non-parsable characters (e.g., newlines)
            p++;
            continue;
        }
        sum += value;
        p = result.ptr;
        // Skip potential newline delimiter
        if (p < end && *p == '\n') {
            p++;
        }
    }
    return sum;
}