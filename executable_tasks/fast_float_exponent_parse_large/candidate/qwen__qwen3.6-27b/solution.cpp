#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        double value = 0.0;
        auto res = fast_float::from_chars(p, end, value);
        sum += value;
        p = res.ptr;
        if (p < end && *p == '\n') {
            ++p;
        }
    }
    return sum;
}