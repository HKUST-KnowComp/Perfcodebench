#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    const size_t total_size = input.size();

    if (total_size == 0) return acc;

    const char* current = p;
    while (current < end) {
        const char* next_nl = static_cast<const char*>(std::memchr(current, '\n', end - current));
        
        if (next_nl) {
            size_t len = next_nl - current;
            acc ^= XXH64(current, len, 0);
            acc *= 1099511628211ULL;
            current = next_nl + 1;
        } else {
            size_t len = end - current;
            acc ^= XXH64(current, len, 0);
            acc *= 1099511628211ULL;
            current = end;
        }
    }
    return acc;
}