#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    double sum = 0.0;
    const char* p   = input.data();
    const char* end = p + input.size();

    while (p < end) {
        double v;
        auto [ptr, ec] = fast_float::from_chars(p, end, v);
        if (ec != std::errc()) break;          // unrecoverable parse error
        sum += v;                              // valid value; accumulate

        // advance past optional newline; both branches are pointer increments
        p = (ptr < end && *ptr == '\n') ? ptr + 1 : ptr;
    }
    return sum;
}