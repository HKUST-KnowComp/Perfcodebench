#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* const end = ptr + input.size();
    double sum = 0.0;
    while (ptr < end) {
        double value;
        auto res = fast_float::from_chars(ptr, end, value);
        if (res.ec) break;
        sum += value;
        ptr = res.ptr;
        if (ptr < end && *ptr == ':') ++ptr;
    }
    return sum;
}