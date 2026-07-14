#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    double value;
    while (p < end) {
        auto res = fast_float::from_chars(p, end, value);
        if (res.ec != std::errc()) {
            break;
        }
        sum += value;
        p = res.ptr;
        // Skip newline(s) to reach next number
        while (p < end && (*p == '\n' || *p == '\r')) {
            ++p;
        }
    }
    return sum;
}
