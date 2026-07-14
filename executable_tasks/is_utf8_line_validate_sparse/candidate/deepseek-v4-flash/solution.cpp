#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* p = input.data();
    const char* const end = p + input.size();

    while (p < end) {
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        if (line_end == nullptr) {
            line_end = end;
        }
        std::size_t line_len = static_cast<std::size_t>(line_end - p);
        if (is_utf8(p, line_len)) {
            ++count;
        }
        p = (line_end == end) ? end : line_end + 1;
    }
    return count;
}
