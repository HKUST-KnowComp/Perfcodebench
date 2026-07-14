#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip whitespace
        while (p < end && (*p == ' ' || (*p >= '\t' && *p <= '\r') || *p == '\n')) {
            ++p;
        }

        if (p >= end) break;

        // Use fast_float for high-performance parsing
        double value = 0.0;
        const char* next = nullptr;
        auto result = fast_float::parse_double(p, end, value, &next);

        if (result == fast_float::error_code::ok) {
            sum += value;
            p = next;
        } else {
            // In case of a parsing error or unexpected character, advance to avoid infinite loop
            ++p;
        }
    }
    return sum;
}