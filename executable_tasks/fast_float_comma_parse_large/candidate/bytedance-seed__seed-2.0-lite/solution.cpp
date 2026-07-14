#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    const char* start = input.data();
    const char* const total_end = start + input.size();
    
    while (start < total_end) {
        // Locate the next comma delimiter for the current number
        const char* comma = start;
        while (comma < total_end && *comma != ',') {
            ++comma;
        }
        
        // Parse the numeric substring with fast_float
        double value;
        auto parse_result = fast_float::from_chars(start, comma, value);
        if (parse_result.ec != std::errc()) {
            // Match original baseline behavior: exit early on parse failure
            break;
        }
        
        sum += value;
        // Advance past the comma to the start of the next number
        start = comma + 1;
    }
    return sum;
}