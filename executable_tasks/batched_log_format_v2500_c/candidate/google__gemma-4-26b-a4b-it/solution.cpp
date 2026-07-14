#include "interface.h"
#include <string>
#include <vector>
#include <charconv>
#include <cstring>

namespace {

// FNV-1a hash implementation optimized for raw byte processing
inline uint64_t fnv1a_update(uint64_t hash, const char* data, size_t len) {
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    // Pre-allocate a large buffer to avoid reallocations during the loop.
    // Estimating ~64-128 bytes per log line is usually safe for typical logs.
    std::string buffer;
    buffer.reserve(ids.size() * 128);

    uint64_t final_hash = 0;
    const size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        
        // Use a local stack buffer for integer conversions to avoid std::to_string allocations
        char int_buf[12]; 

        for (size_t i = 0; i < n; ++i) {
            // Timestamp
            auto [p1, ec1] = std::to_chars(int_buf, int_buf + sizeof(int_buf), timestamps[i]);
            buffer.append(int_buf, p1 - int_buf);
            buffer.push_back('|');

            // ID
            auto [p2, ec2] = std::to_chars(int_buf, int_buf + sizeof(int_buf), ids[i]);
            buffer.append(int_buf, p2 - int_buf);
            buffer.push_back('|');

            // Level
            auto [p3, ec3] = std::to_chars(int_buf, int_buf + sizeof(int_buf), levels[i]);
            buffer.append(int_buf, p3 - int_buf);
            buffer.push_back('|');

            // Message
            buffer.append(messages[i]);
            buffer.push_back('\n');
        }

        // Calculate FNV-1a hash on the resulting buffer
        uint64_t hash = 1469598103934665603ULL;
        hash = fnv1a_update(hash, buffer.data(), buffer.size());
        final_hash = hash;
    }

    return final_hash;
}