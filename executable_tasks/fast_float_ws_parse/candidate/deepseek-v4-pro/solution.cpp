#include "interface.h"
#include <fast_float/fast_float.h>
#include <string>
#include <system_error>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    double value;
    while (p < end) {
        // skip leading whitespace
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
            ++p;
        }
        if (p >= end) break;
        auto res = fast_float::from_chars(p, end, value);
        if (res.ec == std::errc()) {
            sum += value;
            p = res.ptr;
        } else {
            ++p;
        }
    }
    return sum;
}
