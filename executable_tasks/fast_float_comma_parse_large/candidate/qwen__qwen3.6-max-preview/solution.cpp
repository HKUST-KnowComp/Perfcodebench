#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    double val;
    while (p < end) {
        auto res = fast_float::from_chars(p, end, val);
        if (res.ptr == p) break;
        sum += val;
        p = res.ptr;
        if (p < end && *p == ',') ++p;
    }
    return sum;
}