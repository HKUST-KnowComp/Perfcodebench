#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    const char* data = input.data();
    const char* end = data + input.size();
    uint64_t acc = 1469598103934665603ULL;
    const char* p = data;
    while (p < end) {
        const char* q = p;
        while (q < end && *q != '\n') ++q;
        acc ^= XXH64(p, static_cast<size_t>(q - p), 0);
        acc *= 1099511628211ULL;
        p = q + 1;
    }
    return acc;
}