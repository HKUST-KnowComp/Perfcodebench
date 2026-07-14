#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* first = input.data();
    const char* last  = first + input.size();
    double sum = 0.0;
    while (first < last) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(first, last, value);
        if (ec != std::errc()) break;   // no more valid numbers
        sum += value;
        first = (*ptr == '\n') ? ptr + 1 : ptr;
    }
    return sum;
}