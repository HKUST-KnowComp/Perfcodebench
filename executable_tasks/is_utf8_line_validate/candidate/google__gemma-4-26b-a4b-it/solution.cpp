#include "interface.h"
#include "is_utf8.h"
#include <string_view>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    std::string_view sv(input);
    size_t start = 0;
    size_t len = sv.size();

    while (start < len) {
        size_t end = sv.find('\n', start);
        if (end == std::string_view::npos) {
            end = len;
        }

        // Create a view of the current line without copying
        std::string_view line = sv.substr(start, end - start);
        
        // Use the optimized external is_utf8 function
        // We pass the pointer and length to avoid string construction
        if (is_utf8(line.data(), line.size())) {
            count++;
        }

        start = end + 1;
    }

    return count;
}