#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    const uint64_t prime = 1099511628211ULL;

    while (p < end) {
        // Use memchr for high-performance newline detection
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        
        if (line_end) {
            size_t len = static_cast<size_t>(line_end - p);
            acc ^= XXH64(p, len, 0);
            acc *= prime;
            p = line_end + 1;
        } else {
            // Last line without a trailing newline
            size_t len = static_cast<size_t>(end - p);
            acc ^= XXH64(p, len, 0);
            acc *= prime;
            p = end;
        }
    }
    
    // The baseline logic: if the input is empty, the loop doesn't run, returning the initial seed.
    // If the input ends with '\n', the loop processes the last line and p becomes 'end', exiting.
    // The logic matches the baseline exactly.
    return acc;
}