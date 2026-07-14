#include "interface.h"
#include <string>
#include <cctype>
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
    // Create a mutable copy and replace 'd'/'D' with 'e'/'E' in one pass
    std::string buf = input;
    for (char& c : buf) {
        if (c == 'd') c = 'e';
        else if (c == 'D') c = 'E';
    }

    const char* p = buf.c_str();
    const char* const end = p + buf.size();
    double sum = 0.0;

    while (p < end) {
        // Find end of line
        const char* line_end = p;
        while (line_end < end && *line_end != '\n')
            ++line_end;

        // Skip leading whitespace (matching strtod behavior)
        const char* start = p;
        while (start < line_end && std::isspace(static_cast<unsigned char>(*start)))
            ++start;

        // If no digit characters remain, break (same as baseline)
        if (start >= line_end)
            break;

        double value;
        auto result = fast_float::from_chars(start, line_end, value);
        if (result.ptr == start)   // no characters parsed -> break
            break;

        sum += value;
        p = (line_end < end) ? line_end + 1 : line_end;
    }

    return sum;
}
