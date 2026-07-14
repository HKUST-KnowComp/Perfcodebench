#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();

    // Pre-allocate state to avoid repeated initialization overhead
    XXH64_state_t* state = XXH64_createState();
    if (!state) return 0; // Should not happen in this environment

    while (p < end) {
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        
        if (line_end) {
            size_t line_len = static_cast<size_t>(line_end - p);
            // Re-initialize state for each line to match the original XXH64(p, len, 0) behavior
            XXH64_reset(state, 0);
            XXH64_update(state, p, line_len);
            acc ^= XXH64_digest(state);
            
            p = line_end + 1;
        } else {
            // Last line without a trailing newline
            size_t line_len = static_cast<size_t>(end - p);
            XXH64_reset(state, 0);
            XXH64_update(state, p, line_len);
            acc ^= XXH64_digest(state);
            p = end;
        }
        
        acc *= 1099511628211ULL;
    }

    XXH64_freeState(state);
    return acc;
}