#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc() || ptr == p) {
            break;
        }
        sum += value;
        p = ptr;
        // Skip the newline delimiter if present
        if (p < end && *p == '\n') {
            ++p;
        }
    }
    return sum;
}