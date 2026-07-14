#include "interface.h"
#include <fast_float/fast_float.h>
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

        if (p == end) break;

        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == fast_float::error_code::none) {
            sum += value;
            p = ptr;
        } else {
            // If parsing fails (e.g., not a number), skip one character and try again
            // to maintain the behavior of the original baseline.
            ++p;
        }
    }

    return sum;
}