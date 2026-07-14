#include "interface.h"
#include <fast_float/fast_float.h>
#include <cctype>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        while (p < end && static_cast<unsigned char>(*p) <= ' ') ++p;
        if (p >= end) break;
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc{}) {
            ++p;
            continue;
        }
        sum += value;
        p = ptr;
    }
    return sum;
}