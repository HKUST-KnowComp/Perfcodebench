#include "interface.h"
#include <charconv>
#include <vector>
#include <string>
#include <cstring>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= FNV_PRIME;
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations.
    // Max uint32 is 10 digits, max uint16 is 5 digits, plus separators and messages.
    size_t max_line_size = 12 + 12 + 6 + 1 + 1; // timestamps|ids|levels|msg\n
    std::vector<char> buffer;
    buffer.reserve(ids.size() * 128); // Heuristic starting size

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        
        for (size_t i = 0; i < ids.size(); ++i) {
            char temp[32];
            
            // Timestamp
            auto [p1, ec1] = std::to_chars(temp, temp + 32, timestamps[i]);
            buffer.insert(buffer.end(), temp, p1);
            buffer.push_back('|');

            // ID
            auto [p2, ec2] = std::to_chars(temp, temp + 32, ids[i]);
            buffer.insert(buffer.end(), temp, p2);
            buffer.push_back('|');

            // Level
            auto [p3, ec3] = std::to_chars(temp, temp + 32, levels[i]);
            buffer.insert(buffer.end(), temp, p3);
            buffer.push_back('|');

            // Message
            const std::string& msg = messages[i];
            buffer.insert(buffer.end(), msg.begin(), msg.end());
            buffer.push_back('\n');
        }

        // Compute FNV-1a hash on the buffer
        uint64_t current_hash = FNV_OFFSET_BASIS;
        update_hash(current_hash, buffer.data(), buffer.size());
        final_hash = current_hash;
    }

    return final_hash;
}
