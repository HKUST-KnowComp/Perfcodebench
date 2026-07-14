#include "interface.h"
#include <fast_float/fast_float.h>
#include <algorithm>
#include <vector>

double parse_sum(const std::string& input) {
    std::vector<char> buf(input.begin(), input.end());
    std::replace(buf.begin(), buf.end(), 'd', 'e');
    std::replace(buf.begin(), buf.end(), 'D', 'E');

    const char* p = buf.data();
    const char* end = p + buf.size();
    double sum = 0.0;

    while (p < end) {
        // Skip whitespace (newlines, carriage returns, spaces, tabs)
        while (p < end && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t')) {
            ++p;
        }
        if (p >= end) break;

        double value;
        auto res = fast_float::from_chars(p, end, value);
        if (res.ec != std::errc()) {
            break;
        }
        sum += value;
        p = res.ptr;
    }
    return sum;
}
