#include "interface.h"
#include <string>
#include <vector>
#include <charconv>
#include <cstring>

namespace {

// Fast FNV-1a hash implementation
inline uint64_t fnv1a_update(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
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
    
    if (ids.empty()) return 1469598103934665603ULL;

    // Pre-allocate a large buffer to avoid reallocations.
    // Estimate: ~32 bytes per log entry + message length.
    size_t estimated_size = 0;
    for (const auto& msg : messages) {
        estimated_size += msg.size() + 32;
    }
    
    std::string buffer;
    buffer.reserve(estimated_size);

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        
        for (size_t i = 0; i < ids.size(); ++i) {
            char temp[32];
            
            // Timestamp
            auto [p1, ec1] = std::to_chars(temp, temp + 32, timestamps[i]);
            buffer.append(temp, p1 - temp);
            buffer.push_back('|');

            // ID
            auto [p2, ec2] = std::to_chars(temp, temp + 32, ids[i]);
            buffer.append(temp, p2 - temp);
            buffer.push_back('|');

            // Level
            auto [p3, ec3] = std::to_chars(temp, temp + 32, levels[i]);
            buffer.append(temp, p3 - temp);
            buffer.push_back('|');

            // Message
            buffer.append(messages[i]);
            buffer.push_back('\n');
        }
    }

    // Final hash calculation
    uint64_t hash = 1469598103934665603ULL;
    hash = fnv1a_update(hash, buffer.data(), buffer.size());
    
    return hash;
}