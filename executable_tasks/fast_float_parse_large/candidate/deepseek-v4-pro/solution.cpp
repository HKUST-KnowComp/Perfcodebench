#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.c_str();
    const char* const end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        // skip any leading newline delimiters
        while (p < end && *p == '\n') {
            ++p;
        }
        if (p == end) break;
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc{} || ptr == p) {
            // parse failed or no progress
            break;
        }
        sum += value;
        p = ptr;
        // loop back will skip the trailing newline
    }
    return sum;
}