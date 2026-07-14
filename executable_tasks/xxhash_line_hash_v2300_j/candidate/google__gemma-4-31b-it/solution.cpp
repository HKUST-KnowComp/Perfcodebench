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
        // Find the next newline character
        while (end < size && data[end] != '\n') {
            ++end;
        }

        // Calculate hash of the current line slice without allocating a new string
        uint64_t line_hash = XXH64(data + start, end - start, 0);
        
        acc ^= line_hash;
        acc *= 1099511628211ULL;

        // Move start to the character after the newline
        start = end + 1;
    }
    return acc;
}