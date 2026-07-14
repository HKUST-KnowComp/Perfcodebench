#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        auto result = fast_float::from_chars(p, end, value);
        
        if (result.ec != std::errc()) {
            // Skip non-parsable characters (e.g., newlines)
            p++;
            continue;
        }
        
        sum += value;
        p = result.ptr;
        
        // Skip newline if present
        if (p < end && *p == '\n') {
            p++;
        }
    }
    return sum;
}