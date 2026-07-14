#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();

    uint64_t acc = 1469598103934665603ULL; // FNV offset basis
    const uint64_t fnv_prime = 1099511628211ULL;

    while (p < end) {
        size_t remaining = static_cast<size_t>(end - p);
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', remaining));
        size_t len = nl ? static_cast<size_t>(nl - p) : remaining;

        uint64_t h = XXH64(p, len, 0);
        acc ^= h;
        acc *= fnv_prime;

        if (!nl) break;
        p = nl + 1;
    }

    return acc;
}
