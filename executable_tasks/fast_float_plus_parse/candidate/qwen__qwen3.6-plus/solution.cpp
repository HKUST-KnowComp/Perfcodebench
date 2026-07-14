#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* end = ptr + input.size();
    double sum = 0.0;
    while (ptr < end) {
        double val;
        auto res = fast_float::from_chars(ptr, end, val);
        if (res.ec != std::errc()) {
            break;
        }
        sum += val;
        ptr = res.ptr;
        if (ptr < end && *ptr == '\n') {
            ++ptr;
        }
    }
    return sum;
}