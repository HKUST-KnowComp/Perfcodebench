#include "interface.h"
#include "fast_float/fast_float.h"
#include <cctype>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip whitespace
        while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
            ++p;
        }
        if (p >= end) break;

        // Parse double using fast_float
        double value;
        auto result = fast_float::from_chars(p, end, value);
        
        if (result.ec == std::errc()) {
            sum += value;
            p = result.ptr;
        } else {
            // If parsing fails, skip one character to avoid infinite loop
            ++p;
        }
    }
    return sum;
}