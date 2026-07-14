#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <charconv>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        // Skip potential leading whitespace/newlines if any
        if (*p == '\n' || *p == ' ' || *p == '\r' || *p == '\t') {
            p++;
            continue;
        }

        fast_float::from_chars_result result = fast_float::from_chars(p, end, sum, fast_float::chars_format::general);
        
        if (result.ec != std::errc()) {
            break;
        }
        
        p = result.ptr;
        // Advance past the newline character if present
        if (p < end && *p == '\n') {
            p++;
        }
    }
    return sum;
}