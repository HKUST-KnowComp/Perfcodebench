#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <charconv>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip whitespace and newlines
        while (p < end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) {
            p++;
        }
        
        if (p >= end) break;

        // Use fast_float for high-performance parsing
        // fast_float::from_chars is highly optimized for both float and integer representations
        double value;
        auto [ptr, ec] = fast_float::from_chars(p, end, value);

        if (ec == std::errc()) {
            sum += value;
            p = ptr;
        } else {
            // If parsing fails, move forward to avoid infinite loop
            // In the context of this task, we assume valid integer tokens
            p++;
        }
    }

    return sum;
}