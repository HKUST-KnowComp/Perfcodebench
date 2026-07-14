#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        // skip whitespace
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;
        if (p >= end) break;
        double val;
        auto result = fast_float::from_chars(p, end, val);
        if (result.ptr != p) {
            sum += val;
            p = result.ptr;
        } else {
            // skip one character on parse failure
            ++p;
        }
    }
    return sum;
}