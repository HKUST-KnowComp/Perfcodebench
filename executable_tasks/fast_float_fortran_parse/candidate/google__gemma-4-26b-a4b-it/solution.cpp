#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <vector>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    // Use a reusable buffer to avoid repeated allocations
    // The buffer size is chosen to handle reasonably long lines
    std::vector<char> buffer;
    buffer.reserve(128);

    while (p < end) {
        const char* line_start = p;
        const char* line_end = p;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }

        size_t len = line_end - line_start;
        if (len > 0) {
            buffer.resize(len);
            for (size_t i = 0; i < len; ++i) {
                char c = line_start[i];
                // Convert Fortran 'd'/'D' exponent notation to C++ 'e'/'E'
                if (c == 'd' || c == 'D') {
                    buffer[i] = 'e';
                } else {
                    buffer[i] = c;
                }
            }

            double value;
            auto [ptr, ec] = fast_float::from_chars(buffer.data(), buffer.data() + len, value);
            if (ec == fast_float::error_code::none) {
                sum += value;
            }
        }

        p = (line_end < end) ? line_end + 1 : line_end;
    }

    return sum;
}