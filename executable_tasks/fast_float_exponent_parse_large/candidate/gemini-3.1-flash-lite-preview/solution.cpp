#include "interface.h"
#include "fast_float/fast_float.h"
#include <string_view>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    std::string_view sv(input);
    const char* ptr = sv.data();
    const char* end = ptr + sv.size();

    while (ptr < end) {
        // Skip potential leading whitespace or empty lines
        if (*ptr == '\n' || *ptr == '\r' || *ptr == ' ') {
            ptr++;
            continue;
        }

        double value;
        auto result = fast_float::from_chars(ptr, end, value);
        
        if (result.ec != std::errc()) {
            // If parsing fails, skip the current character to avoid infinite loop
            ptr++;
            continue;
        }

        sum += value;
        ptr = result.ptr;
    }

    return sum;
}