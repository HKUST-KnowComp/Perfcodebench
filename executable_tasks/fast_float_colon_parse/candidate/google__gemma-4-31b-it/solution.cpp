#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value = 0.0;
        const char* next = fast_float::parse_double(p, end, value);
        
        if (next == p) {
            // Handle cases where parsing fails or we hit an unexpected character
            // In the context of this task, we skip the character and move on
            p++;
            continue;
        }
        
        sum += value;
        p = next;
        
        // Skip the colon delimiter
        if (p < end && *p == ':') {
            p++;
        }
    }
    return sum;
}