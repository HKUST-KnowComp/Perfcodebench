#include "interface.h"
#include "is_utf8.h"
#include <string_view>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    std::string_view sv(input);
    
    size_t start = 0;
    size_t end = 0;
    size_t n = sv.size();

    while (start < n) {
        // Find the next newline
        end = sv.find('\n', start);
        
        if (end == std::string_view::npos) {
            // Last segment (no trailing newline)
            if (is_utf8::is_valid(sv.data() + start, n - start)) {
                count++;
            }
            break;
        } else {
            // Segment between start and newline
            if (is_utf8::is_valid(sv.data() + start, end - start)) {
                count++;
            }
            start = end + 1;
        }
    }

    return count;
}