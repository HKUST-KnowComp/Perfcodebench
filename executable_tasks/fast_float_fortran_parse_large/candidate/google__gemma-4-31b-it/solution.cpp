#include "interface.h"
#include <fast_float/fast_float.h>
#include <vector>
#include <string>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    
    // Buffer to handle Fortran 'D'/'d' exponents without allocating std::string
    // Max length of a double string is typically < 64 chars
    char buf[128];

    while (p < end) {
        const char* line_end = p;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }

        size_t len = line_end - p;
        if (len > 0) {
            bool needs_fix = false;
            for (size_t i = 0; i < len; ++i) {
                if (p[i] == 'd' || p[i] == 'D') {
                    needs_fix = true;
                    break;
                }
            }

            if (!needs_fix) {
                double val;
                auto result = fast_float::from_chars(p, line_end, val);
                if (result.ptr != p) {
                    sum += val;
                }
            } else {
                // Copy to buffer and replace D/d with E/e
                size_t copy_len = (len < 127) ? len : 127;
                for (size_t i = 0; i < copy_len; ++i) {
                    char c = p[i];
                    if (c == 'd') buf[i] = 'e';
                    else if (c == 'D') buf[i] = 'E';
                    else buf[i] = c;
                }
                buf[copy_len] = '\0';
                
                double val;
                auto result = fast_float::from_chars(buf, buf + copy_len, val);
                if (result.ptr != buf) {
                    sum += val;
                }
            }
        }
        p = (line_end < end) ? line_end + 1 : end;
    }
    return sum;
}