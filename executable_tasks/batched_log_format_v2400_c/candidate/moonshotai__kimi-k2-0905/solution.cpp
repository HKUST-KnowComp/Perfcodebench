#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_buffer(const char* data, size_t len) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
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
    int iters)
{
    const size_t n = ids.size();

    // Pre-compute the exact byte sequence of the entire output once.
    std::vector<char> buffer;
    buffer.reserve(n * 64); // rough guess to avoid reallocations

    for (size_t i = 0; i < n; ++i) {
        // Timestamp
        char num[16];
        int len = std::snprintf(num, sizeof(num), "%u", timestamps[i]);
        buffer.insert(buffer.end(), num, num + len);
        buffer.push_back('|');

        // ID
        len = std::snprintf(num, sizeof(num), "%u", ids[i]);
        buffer.insert(buffer.end(), num, num + len);
        buffer.push_back('|');

        // Level
        len = std::snprintf(num, sizeof(num), "%u", levels[i]);
        buffer.insert(buffer.end(), num, num + len);
        buffer.push_back('|');

        // Message
        const std::string& msg = messages[i];
        buffer.insert(buffer.end(), msg.data(), msg.data() + msg.size());
        buffer.push_back('\n');
    }

    const size_t totalLen = buffer.size();
    const char* const data = buffer.data();

    // Compute the checksum for the required number of iterations.
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_buffer(data, totalLen);
    }
    return hash;
}