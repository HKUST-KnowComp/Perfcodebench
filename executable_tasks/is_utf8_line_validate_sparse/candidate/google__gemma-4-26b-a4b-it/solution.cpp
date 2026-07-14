#include "interface.h"
#include "is_utf8.h"
#include <string_view>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    std::string_view sv(input);
    size_t start = 0;
    size_t end = 0;

    while ((end = sv.find('\n', start)) != std::string_view::npos) {
        std::string_view line = sv.substr(start, end - start);
        if (is_utf8(line.data(), line.size())) {
            count++;
        }
        start = end + 1;
    }

    // Handle the last segment if the string doesn't end with a newline
    if (start < sv.size()) {
        std::string_view line = sv.substr(start);
        if (is_utf8(line.data(), line.size())) {
            count++;
        }
    }

    return count;
}