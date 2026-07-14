#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    size_t size = input.size();
    size_t start = 0;

    while (start < size) {
        // Use memchr for vectorized newline searching
        const char* next_newline = static_cast<const char*>(std::memchr(data + start, '\n', size - start));
        size_t end;
        
        if (next_newline) {
            end = static_cast<size_t>(next_newline - data);
        } else {
            end = size;
        }

        // Hash the slice directly without creating a std::string object
        size_t line_len = end - start;
        if (line_len > 0) {
            acc ^= XXH64(data + start, line_len, 0);
            acc *= 1099511628211ULL;
        } else if (end == size && start == size) {
            // Handle empty input case if necessary, though loop condition covers it
        } else if (end < size && start == end) {
            // Handle empty line (just a newline character)
            acc ^= XXH64(data + start, 0, 0);
            acc *= 1099511628211ULL;
        }

        if (next_newline) {
            start = end + 1;
        } else {
            break;
        }
    }
    return acc;
}