#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* const end = ptr + input.size();
    double sum = 0.0;
    while (ptr < end) {
        while (ptr < end && *ptr <= 32) ++ptr;
        if (ptr == end) break;
        
        double value;
        auto res = fast_float::from_chars(ptr, end, value);
        if (res.ptr != ptr) {
            sum += value;
            ptr = res.ptr;
        } else {
            ++ptr;
        }
    }
    return sum;
}