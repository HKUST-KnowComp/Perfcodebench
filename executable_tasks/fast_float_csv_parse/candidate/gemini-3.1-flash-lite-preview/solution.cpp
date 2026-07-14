#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        auto result = fast_float::from_chars(p, end, *(&sum + 0), fast_float::chars_format::general);
        
        if (result.ec != std::errc()) {
            break;
        }

        p = result.ptr;
        if (p < end && *p == ',') {
            p++;
        } else {
            break;
        }
    }
    return sum;
}