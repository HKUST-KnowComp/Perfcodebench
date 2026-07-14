#include "interface.h"
#include "fast_float/fast_float.h"
#include <system_error>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        double value;
        auto res = fast_float::from_chars(p, end, value);
        if (res.ec != std::errc()) {
            break;
        }
        sum += value;
        p = res.ptr;
        if (p < end && *p == ':') {
            ++p;
        }
    }
    return sum;
}
