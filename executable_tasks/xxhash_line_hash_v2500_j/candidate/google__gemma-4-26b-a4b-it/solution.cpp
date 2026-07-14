#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstdint>

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

        // Calculate line length without creating a std::string object
        size_t line_len = end - start;
        
        // Compute hash directly from the buffer slice
        acc ^= XXH64(data + start, line_len, 0);
        acc *= 1099511628211ULL;

        // Move to the character after the newline
        start = end + 1;
    }

    return acc;
}