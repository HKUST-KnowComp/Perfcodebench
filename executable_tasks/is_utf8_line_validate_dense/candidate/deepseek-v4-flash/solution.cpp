#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* p = input.data();
    const char* const end = p + input.size();

    while (p < end) {
        const char* const line_start = p;
        const char* const line_end = static_cast<const char*>(
            std::memchr(p, '\n', end - p));
        if (line_end) {
            p = line_end + 1;          // skip newline
        } else {
            p = end;                   // last line without newline
        }
        const std::size_t len = static_cast<std::size_t>(line_end ? line_end - line_start : end - line_start);
        if (is_valid_utf8(line_start, len)) {
            ++count;
        }
    }
    return count;
}
