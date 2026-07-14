#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    const char* first = input.data();
    const char* last  = first + input.size();
    while (first != last) {
        double v;
        auto [ptr, ec] = fast_float::from_chars(first, last, v);
        if (ec != std::errc()) break;          // malformed or empty
        sum += v;
        first = ptr;
        if (first != last && *first == '|') ++first;
    }
    return sum;
}