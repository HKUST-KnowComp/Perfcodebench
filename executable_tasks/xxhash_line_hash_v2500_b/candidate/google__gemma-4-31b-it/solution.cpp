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
        const char* current = data + start;
        size_t len = 0;
        
        // Fast scan for newline
        while (start + len < size && data[start + len] != '\n') {
            ++len;
        }

        // Hash the slice directly without creating a substring
        acc ^= XXH64(current, len, 0);
        acc *= 1099511628211ULL;

        start += len + 1;
    }
    return acc;
}