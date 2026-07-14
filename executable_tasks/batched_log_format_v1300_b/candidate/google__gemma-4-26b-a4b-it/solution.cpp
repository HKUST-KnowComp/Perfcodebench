#include "interface.h"
#include <string>
#include <vector>
#include <charconv>
#include <cstring>

namespace {

// Fast FNV-1a hash implementation
struct FNV1a {
    uint64_t hash = 1469598103934665603ULL;
    static constexpr uint64_t prime = 1099511628211ULL;

    inline void update(const char* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
            hash *= prime;
        }
    }

    inline void update_char(char c) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= prime;
    }
};

// Fast integer to string conversion using std::to_chars
// This avoids the heap allocation of std::to_string
inline size_t append_int(char* buf, uint32_t val) {
    auto [ptr, ec] = std::to_chars(buf, buf + 12, val);
    return ptr - buf;
} 

inline size_t append_int_u16(char* buf, uint16_t val) {
    auto [ptr, ec] = std::to_chars(buf, buf + 6, val);
    return ptr - buf;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    FNV1a hasher;
    // Pre-allocate a buffer for a single line to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message
    // We'll use a reasonably large buffer for the line components
    char line_buf[4096]; 

    for (int iter = 0; iter < iters; ++iter) {
        // Reset hash for each iteration as per original logic
        hasher.hash = 1469598103934665603ULL;

        for (std::size_t i = 0; i < ids.size(); ++i) {
            // 1. Timestamp
            size_t len = append_int(line_buf, timestamps[i]);
            hasher.update(line_buf, len);
            
            // 2. Separator |
            hasher.update_char('|');

            // 3. ID
            len = append_int(line_buf, ids[i]);
            hasher.update(line_buf, len);

            // 4. Separator |
            hasher.update_char('|');

            // 5. Level
            len = append_int_u16(line_buf, levels[i]);
            hasher.update(line_buf, len);

            // 6. Separator |
            hasher.update_char('|');

            // 7. Message
            const std::string& msg = messages[i];
            hasher.update(msg.data(), msg.size());

            // 8. Newline \
            hasher.update_char('\n');
        }
    }

    return hasher.hash;
}
