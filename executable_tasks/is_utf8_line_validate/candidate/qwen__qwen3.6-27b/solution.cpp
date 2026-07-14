#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <cstddef>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* data = input.data();
    const char* end = data + input.size();
    const char* start = data;
    while (start < end) {
        const char* line_end = start;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        if (is_utf8(start, line_end - start)) {
            ++count;
        }
        start = line_end + 1;
    }
    return count;
}