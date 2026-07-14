#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* ptr = input.data();
    const char* end_ptr = ptr + input.size();

    while (ptr < end_ptr) {
        const char* line_start = ptr;
        // Use memchr for highly optimized newline scanning
        const char* line_end = static_cast<const char*>(std::memchr(ptr, '\n', end_ptr - ptr));
        
        size_t line_len;
        if (line_end) {
            line_len = line_end - line_start;
            ptr = line_end + 1;
        } else {
            line_len = end_ptr - line_start;
            ptr = end_ptr;
        }

        // Hash the segment directly without creating a std::string object
        acc ^= XXH64(line_start, line_len, 0);
        acc *= 1099511628211ULL;
    }

    return acc;
}