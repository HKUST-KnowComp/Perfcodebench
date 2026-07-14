#include "interface.h"
#include "is_utf8.h"
#include <string_view>

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library is expected to provide a high-performance validation function.
 * We scan for newlines and validate each segment using the optimized validator.
 */
std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    std::string_view data(input);
    std::size_t start = 0;
    std::size_t pos = 0;

    while ((pos = data.find('\n', start)) != std::string_view::npos) {
        std::string_view line = data.substr(start, pos - start);
        if (is_utf8(line.data(), line.size())) {
            count++;
        }
        start = pos + 1;
    }

    // Handle the last line if it doesn't end with a newline
    if (start < data.size()) {
        std::string_view line = data.substr(start);
        if (is_utf8(line.data(), line.size())) {
            count++;
        }
    } else if (start == data.size() && !data.empty() && data.back() == '\n') {
        // Edge case: input ends with a newline, the last segment is empty and valid
        count++;
    }

    return count;
}