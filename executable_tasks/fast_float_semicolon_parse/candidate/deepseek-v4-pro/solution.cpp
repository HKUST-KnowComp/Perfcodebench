#include "interface.h"
#include <fast_float/fast_float.h>
#include <system_error>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();
    double sum = 0.0;
    while (p < end) {
        double val;
        auto result = fast_float::from_chars(p, end, val);
        if (result.ec != std::errc() || result.ptr == p) {
            break;
        }
        sum += val;
        p = result.ptr;
        if (p < end && *p == ';') {
            ++p;
        }
    }
    return sum;
}
