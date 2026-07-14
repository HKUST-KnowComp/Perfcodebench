#include "interface.h"
#include "fast_float/fast_float.h"
#include <system_error>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        while (p < end && *p <= 32) ++p;
        if (p >= end) break;
        
        double value;
        auto result = fast_float::from_chars(p, end, value);
        if (result.ec == std::errc{}) {
            sum += value;
            p = result.ptr;
        } else {
            ++p;
        }
    }
    return sum;
}
