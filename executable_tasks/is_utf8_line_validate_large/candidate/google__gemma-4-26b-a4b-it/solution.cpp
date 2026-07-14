#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t valid_count = 0;
    const char* ptr = input.data();
    const char* const end = ptr + input.size();

    while (ptr < end) {
        const char* line_start = ptr;
        // Find the next newline character
        const char* line_end = static_cast<const char*>(std::memchr(ptr, '\n', end - ptr));
        
        if (line_end == nullptr) {
            line_end = end;
        } else {
            // Move ptr past the newline for the next iteration
            ptr = line_end + 1;
        }

        // Validate the segment [line_start, line_end)
        // is_utf8::is_valid expects a pointer and a length
        if (is_utf8::is_valid(line_start, static_cast<std::size_t>(line_end - line_start))) {
            valid_count++;
        }

        // If we hit the end of the string, break
        if (line_end == end) {
            break;
        }
    }

    return valid_count;
}