#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        double value = 0.0;
        auto result = fast_float::from_chars(p, end, value);
        if (result.ptr == p) break;
        sum += value;
        p = result.ptr;
        if (p < end && *p == ',') ++p;
    }
    return sum;
}
