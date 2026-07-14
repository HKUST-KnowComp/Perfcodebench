#include "interface.h"
#include <fast_float/fast_float.h>
#include <cstdint>

double parse_sum(const std::string& input) {
    const char* p   = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // skip whitespace
        while (p < end && static_cast<unsigned char>(*p) <= ' ') ++p;
        if (p == end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc{}) {          // invalid number
            ++p;
            continue;
        }
        sum += value;
        p = ptr;
    }
    return sum;
}