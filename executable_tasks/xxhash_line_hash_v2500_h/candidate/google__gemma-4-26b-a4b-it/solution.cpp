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
        // Find the next newline character
        size_t end = start;
        while (end < size && data[end] != '\n') {
            ++end;
        }

        // Calculate line length
        size_t line_len = end - start;

        // Hash the segment directly without creating a std::string object
        // This avoids the overhead of substr() and heap allocation
        acc ^= XXH64(data + start, line_len, 0);
        acc *= 1099511628211ULL;

        // Move start to the character after the newline
        start = end + 1;
    }

    return acc;
}