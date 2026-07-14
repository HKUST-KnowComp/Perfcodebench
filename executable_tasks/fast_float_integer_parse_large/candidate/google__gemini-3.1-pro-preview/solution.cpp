#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <system_error>

/**
 * Optimized parse_sum implementation.
 * 
 * Performance improvements:
 * 1. Replaced std::strtod with fast_float::from_chars, which is significantly faster
 *    and avoids locale-related overhead.
 * 2. Used a tight loop to skip whitespace/newlines between tokens.
 * 3. Avoided unnecessary string copies or complex iterator logic.
 */
double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* const end = ptr + input.size();
    double total_sum = 0.0;
    double current_val = 0.0;

    while (ptr < end) {
        // Skip leading whitespace and newlines
        while (ptr < end && static_cast<unsigned char>(*ptr) <= 32) {
            ptr++;
        }

        if (ptr >= end) {
            break;
        }

        // Parse the next number using fast_float
        auto result = fast_float::from_chars(ptr, end, current_val);
        
        if (result.ec == std::errc()) {
            total_sum += current_val;
            ptr = result.ptr;
        } else {
            // If parsing fails for a specific token, skip to the next character
            // to avoid an infinite loop, though the task implies valid input.
            ptr++;
        }
    }

    return total_sum;
}