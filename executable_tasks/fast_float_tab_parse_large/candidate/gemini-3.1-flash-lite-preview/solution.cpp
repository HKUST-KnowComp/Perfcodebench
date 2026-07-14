#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        fast_float::from_chars_result result = fast_float::from_chars(p, end, sum, fast_float::chars_format::general);
        
        if (result.ec != std::errc()) {
            break;
        }
        
        p = result.ptr;
        // Skip tab delimiter if present
        if (p < end && *p == '\t') {
            p++;
        }
    }
    return sum;
}