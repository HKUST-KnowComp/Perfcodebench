#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* end = ptr + input.size();
    double sum = 0.0;
    
    while (ptr < end) {
        double value;
        auto result = fast_float::from_chars(ptr, end, value);
        if (result.ec != std::errc{}) {
            break;
        }
        sum += value;
        ptr = result.ptr;
        if (ptr < end && *ptr == ';') {
            ++ptr;
        }
    }
    return sum;
}