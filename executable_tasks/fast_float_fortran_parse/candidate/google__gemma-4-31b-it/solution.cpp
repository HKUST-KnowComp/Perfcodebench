#include "interface.h"
#include <fast_float/fast_float.h>
#include <vector>
#include <string>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    
    // Buffer to handle Fortran 'D'/'d' exponents without allocating a new string per line
    char buffer[64];

    while (p < end) {
        const char* line_start = p;
        const char* line_end = p;
        while (line_end < end && *line_end != '\n') {
            line_end++;
        }

        size_t len = line_end - line_start;
        if (len > 0) {
            bool needs_conversion = false;
            for (size_t i = 0; i < len; ++i) {
                if (line_start[i] == 'D' || line_start[i] == 'd') {
                    needs_conversion = true;
                    break;
                }
            }

            if (!needs_conversion) {
                double val;
                auto result = fast_float::from_chars_fast(line_start, line_end, val);
                if (result.ptr != line_start) {
                    sum += val;
                }
            } else {
                // Handle Fortran D/d exponent by copying to a small stack buffer
                size_t copy_len = (len < sizeof(buffer)) ? len : sizeof(buffer) - 1;
                for (size_t i = 0; i < copy_len; ++i) {
                    char c = line_start[i];
                    buffer[i] = (c == 'D' || c == 'd') ? 'E' : c;
                }
                buffer[copy_len] = '\0';
                
                double val;
                auto result = fast_float::from_chars_fast(buffer, buffer + copy_len, val);
                if (result.ptr != buffer) {
                    sum += val;
                }
            }
        }
        p = (line_end < end) ? line_end + 1 : end;
    }
    return sum;
}