#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <system_error>

/**
 * Optimized implementation of parse_sum using the fast_float library.
 * fast_float::from_chars is significantly faster than std::strtod because it is 
 * locale-independent and uses advanced algorithms for float parsing.
 */
double parse_sum(const std::string& input) {
    const char* ptr = input.data();
    const char* const end = ptr + input.size();
    double total_sum = 0.0;
    double current_value = 0.0;

    while (ptr < end) {
        // fast_float::from_chars parses the double and returns a result containing
        // the pointer to the first character not part of the number.
        auto result = fast_float::from_chars(ptr, end, current_value);
        
        if (result.ec == std::errc()) {
            total_sum += current_value;
            ptr = result.ptr;
        } else {
            // If parsing failed, we might be at a delimiter or invalid char.
            // In the context of this task, we expect pipe-delimited doubles.
            // If from_chars didn't consume anything, we must advance manually.
            if (ptr == result.ptr) {
                ++ptr;
            }
        }

        // Skip the pipe delimiter if present.
        if (ptr < end && *ptr == '|') {
            ++ptr;
        }
    }

    return total_sum;
}