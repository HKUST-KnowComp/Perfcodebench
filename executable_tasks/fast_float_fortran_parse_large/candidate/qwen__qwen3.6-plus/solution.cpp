#include "interface.h"
#include <fast_float/fast_float.h>
#include <string>
#include <system_error>

double parse_sum(const std::string& input) {
    std::string buf = input;
    for (char& c : buf) {
        if (c == 'd' || c == 'D') {
            c = 'e';
        }
    }

    const char* ptr = buf.data();
    const char* end = ptr + buf.size();
    double sum = 0.0;

    while (ptr < end) {
        const char* line_end = ptr;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }

        const char* num_start = ptr;
        while (num_start < line_end && (*num_start == ' ' || *num_start == '\t' || *num_start == '\r')) {
            ++num_start;
        }

        if (num_start < line_end) {
            double val;
            auto res = fast_float::from_chars(num_start, line_end, val);
            if (res.ec == std::errc()) {
                sum += val;
            }
        }

        ptr = line_end + 1;
    }

    return sum;
}