#include "interface.h"
#include <string>
#include <cstddef>
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* data = input.data();
    std::size_t len = input.size();
    std::size_t pos = 0;
    while (pos < len) {
        // Find end of current line (newline or end)
        std::size_t end = pos;
        while (end < len && data[end] != '\n') {
            ++end;
        }
        // Validate the line (from pos to end) using fast UTF-8 check
        if (is_valid_utf8(reinterpret_cast<const unsigned char*>(data + pos), end - pos)) {
            ++count;
        }
        // Move past the newline character
        pos = end + 1;
    }
    return count;
}
