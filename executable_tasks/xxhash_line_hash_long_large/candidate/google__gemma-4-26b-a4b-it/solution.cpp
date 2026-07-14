#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();

    while (p < end) {
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        
        if (line_end) {
            size_t len = static_cast<size_t>(line_end - p);
            acc ^= XXH64(p, len, 0);
            acc *= 1099511628211ULL;
            p = line_end + 1;
        } else {
            // No more newlines, process the remaining part of the string
            size_t len = static_cast<size_t>(end - p);
            acc ^= XXH64(p, len, 0);
            acc *= 1099511628211ULL;
            break;
        }
    }
    return acc;
}