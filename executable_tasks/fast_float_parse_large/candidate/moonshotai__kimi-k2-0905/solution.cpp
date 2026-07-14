#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    const char* p = input.data();
    const char* const end = p + input.size();

    while (p < end) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc()) break;   // invalid or empty
        sum += value;
        p = ptr;
        // skip optional newline
        if (p < end && *p == '\n') ++p;
    }
    return sum;
}