#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_digits(const char* start, const char* end) {
    int val = 0;
    while (start < end) {
        val = val * 10 + (*start++ - '0');
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        // Locate first & separating u=... and score=...
        const char* amp1 = row;
        while (*amp1 != '&') amp1++;
        // Parse u value, skip leading "u="
        int u = parse_digits(row + 2, amp1);
        // Locate second & separating score=... and flags=...
        const char* amp2 = amp1 + 1;
        while (*amp2 != '&') amp2++;
        // Parse s value, skip leading "score="
        int s = parse_digits(amp1 + 7, amp2);
        // Locate end of the final flags parameter
        const char* end_row = amp2 + 1;
        while (*end_row) end_row++;
        // Parse f value, skip leading "flags="
        int f = parse_digits(amp2 + 6, end_row);
        // Exact same checksum logic as original baseline
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}