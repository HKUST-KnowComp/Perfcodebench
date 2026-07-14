#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* const start = input.data();
    const char* const end = start + input.size();
    const char* p = start;

    while (p < end) {
        const char* line_start = p;
        // Fast scan for newline
        while (p < end && *p != '\n') {
            ++p;
        }

        // Validate the segment between line_start and p
        // is_utf8::is_valid is assumed to be the optimized implementation
        if (is_utf8::is_valid(line_start, static_cast<std::size_t>(p - line_start))) {
            count++;
        }

        // Move past the newline character if we found one
        if (p < end && *p == '\n') {
            p++;
        }
    }

    return count;
}