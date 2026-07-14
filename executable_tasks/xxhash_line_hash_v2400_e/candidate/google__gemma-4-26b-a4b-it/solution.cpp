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
        // Use find to leverage optimized memchr/SIMD implementations for newline detection
        size_t end = input.find('\n', start);
        
        size_t line_len;
        if (end == std::string::npos) {
            line_len = size - start;
            end = size; // Break condition
        } else {
            line_len = end - start;
        }

        // Avoid std::string substr() to prevent heap allocation and copying
        // Pass raw pointer and length directly to XXH64
        acc ^= XXH64(data + start, line_len, 0);
        acc *= 1099511628211ULL;

        if (end == size) break;
        start = end + 1;
    }

    return acc;
}