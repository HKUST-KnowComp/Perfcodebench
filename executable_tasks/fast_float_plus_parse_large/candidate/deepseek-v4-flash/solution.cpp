#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        // skip whitespace (like strtod does)
        while (p != end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
            ++p;
        }
        if (p == end) break;
        double value;
        auto result = fast_float::from_chars(p, end, value);
        if (result.ptr == p) break;  // no parse progress
        sum += value;
        p = result.ptr;
    }
    return sum;
}
