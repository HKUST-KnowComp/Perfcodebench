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
        const char* current_ptr = data + start;
        const char* end_ptr = static_cast<const char*>(std::memchr(current_ptr, '\n', size - start));
        
        size_t len;
        if (end_ptr) {
            len = end_ptr - current_ptr;
            start += len + 1;
        } else {
            len = size - start;
            start = size;
        }

        acc ^= static_cast<uint64_t>(XXH64(current_ptr, len, 0));
        acc *= 1099511628211ULL;
    }
    return acc;
}