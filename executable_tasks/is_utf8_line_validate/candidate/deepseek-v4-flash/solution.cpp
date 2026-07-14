#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const auto* data = input.data();
    const auto size = input.size();
    std::size_t pos = 0;
    while (pos < size) {
        std::size_t line_end = pos;
        while (line_end < size && data[line_end] != '\n') {
            ++line_end;
        }
        if (is_utf8(data + pos, line_end - pos)) {
            ++count;
        }
        pos = line_end + 1;
    }
    return count;
}
