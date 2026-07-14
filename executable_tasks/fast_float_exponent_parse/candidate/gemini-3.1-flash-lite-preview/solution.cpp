#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip potential leading whitespace if necessary, though input is newline-delimited
        while (p < end && (*p == '\n' || *p == '\r' || *p == ' ')) {
            p++;
        }
        if (p >= end) break;

        double value;
        auto result = fast_float::from_chars(p, end, value);
        
        if (result.ec != std::errc()) {
            break;
        }
        
        sum += value;
        p = result.ptr;
    }
    return sum;
}