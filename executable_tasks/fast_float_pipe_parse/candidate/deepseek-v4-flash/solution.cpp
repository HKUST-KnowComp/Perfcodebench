// Optimized implementation using fast_float library
#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        auto result = fast_float::from_chars(p, end, value);
        if (result.ec != std::errc{}) {
            break;                               // parse failure
        }
        sum += value;
        p = result.ptr;
        // skip the '|' delimiter (if any)
        if (p < end && *p == '|') {
            ++p;
        }
    }
    return sum;
}
