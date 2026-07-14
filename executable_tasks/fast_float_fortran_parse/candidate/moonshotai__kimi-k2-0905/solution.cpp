#include "interface.h"
#include <fast_float/fast_float.h>
#include <cstddef>

double parse_sum(const std::string& input) {
    const char* p   = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // skip leading whitespace/newlines
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) ++p;
        if (p >= end) break;

        const char* num_start = p;
        // find end of line or space
        while (p < end && *p != '\n' && *p != '\r') ++p;
        const char* num_end = p;

        double v = 0.0;
        auto [ptr, ec] = fast_float::from_chars(num_start, num_end, v);
        if (ec == std::errc()) {
            sum += v;
        }
        // else: non-numeric token, skip

        // skip newline if present
        if (p < end && (*p == '\n' || *p == '\r')) ++p;
    }
    return sum;
}