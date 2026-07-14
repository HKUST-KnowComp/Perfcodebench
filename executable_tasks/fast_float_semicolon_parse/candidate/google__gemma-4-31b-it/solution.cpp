#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double value;
        const char* next = fast_float::from_chars(p, end, value);
        
        if (next == p) {
            // Handle cases where the pointer doesn't advance (e.g., trailing semicolons or invalid chars)
            if (*p == ';') {
                p++;
                continue;
            }
            break;
        }
        
        sum += value;
        
        // Advance past the parsed number and the optional semicolon delimiter
        if (next < end && *next == ';') {
            p = next + 1;
        } else {
            p = next;
        }
    }
    return sum;
}