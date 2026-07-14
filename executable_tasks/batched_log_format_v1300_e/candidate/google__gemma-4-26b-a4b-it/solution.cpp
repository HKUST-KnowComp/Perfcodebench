#include "interface.h"
#include <string>
#include <vector>
#include <charconv>
#include <cstring>

namespace {

// Fast FNV-1a implementation
inline uint64_t fnv1a_update(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<unsigned char>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer to string conversion using std::to_chars
inline char* write_int(char* ptr, uint32_t val) {
    auto [p, ec] = std::to_chars(ptr, ptr + 12, val);
    return p;
}

inline char* write_int(char* ptr, uint16_t val) {
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
    
    if (ids.empty()) return 1469598103934665603ULL;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and newline
    // We also account for the message length.
    size_t total_max_size = 0;
    for (const auto& msg : messages) {
        total_max_size += 12 + 7 + 6 + msg.size() + 1;
    }

    std::vector<char> buffer(total_max_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (std::size_t i = 0; i < ids.size(); ++i) {
            ptr = write_int(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_int(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_int(ptr, levels[i]);
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }
        
        // Calculate hash on the populated portion of the buffer
        final_hash = fnv1a_update(1469598103934665603ULL, buffer.data(), ptr - buffer.data());
    }

    return final_hash;
}