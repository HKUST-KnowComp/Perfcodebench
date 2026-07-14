#include "interface.h"
#include "fast_float/fast_float.h"

inline bool is_whitespace(char c) {
    switch (c) {
        case ' ': case '\t': case '\n': case '\r': case '\v': case '\f':
            return true;
        default:
            return false;
    }
}

double parse_sum(const std::string& input) {
    const char* p = input.c_str();
    const char* end = p + input.size();
    double sum = 0.0;
    while (true) {
        while (p < end && is_whitespace(*p)) ++p;
        if (p >= end) break;
        double value;
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
