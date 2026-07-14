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
        size_t end = start;
        // Fast scan for newline
        while (end < size && data[end] != '\n') {
            ++end;
        }

        // Hash the segment directly without creating a std::string object
        size_t line_len = end - start;
        if (line_len > 0) {
            acc ^= XXH64(data + start, line_len, 0);
            acc *= 1099511628211ULL;
        } else if (end < size && data[end] == '\n') {
            // Handle empty line case (e.g., double newline)
            // The original code: line = "", XXH64("", 0) is called.
            acc ^= XXH64(data + start, 0, 0);
            acc *= 1099511628211ULL;
        }

        start = end + 1;
    }

    return acc;
}