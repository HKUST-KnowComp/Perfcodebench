#include "interface.h"
#include <string>
#include <vector>
#include <charconv>

namespace {

inline uint64_t checksum_text(const std::string& text) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : text) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer to string conversion using std::to_chars
// This avoids the overhead of std::to_string and locale handling
inline char* append_int(char* ptr, uint32_t val) {
    auto [p, ec] = std::to_chars(ptr, ptr + 12, val);
    return p;
}

inline char* append_int(char* ptr, uint16_t val) {
    auto [p, ec] = std::to_chars(ptr, ptr + 6, val);
    return p;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    size_t n = ids.size();
    // Pre-calculate a reasonable buffer size to minimize reallocations
    // Avg line: ~10 (ts) + 1 (|) + 10 (id) + 1 (|) + 5 (lvl) + 1 (|) + 30 (msg) + 1 (\n) = ~58
    size_t estimated_size = n * 64;
    std::string buffer;
    buffer.reserve(estimated_size);

    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        
        // We use a temporary char buffer to build the string to avoid 
        // the overhead of std::string::append for every small component.
        // However, for simplicity and safety, we'll use a local char array 
        // and append to the main buffer.
        char temp[128]; 

        for (size_t i = 0; i < n; ++i) {
            char* p = temp;
            p = append_int(p, timestamps[i]);
            *p++ = '|';
            p = append_int(p, ids[i]);
            *p++ = '|';
            p = append_int(p, levels[i]);
            *p++ = '|';
            
            // Append the formatted prefix
            buffer.append(temp, p - temp);
            // Append the message
            buffer.append(messages[i]);
            // Append newline
            buffer.push_back('\n');
        }
        last_hash = checksum_text(buffer);
    }

    return last_hash;
}