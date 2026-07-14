#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion for unsigned integers
inline char* append_uint(char* ptr, uint32_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[10];
    int len = 0;
    while (val > 0) {
        buf[len++] = static_cast<char>('0' + (val % 10));
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

inline char* append_uint16(char* ptr, uint16_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[5];
    int len = 0;
    while (val > 0) {
        buf[len++] = static_cast<char>('0' + (val % 10));
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

inline uint64_t compute_fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
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

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and newline
    // Max message length is unknown, so we sum them up.
    size_t total_msg_len = 0;
    for (const auto& m : messages) total_msg_len += m.length();
    
    size_t max_buffer_size = total_msg_len + (ids.size() * 30); 
    std::vector<char> buffer(max_buffer_size);

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (std::size_t i = 0; i < ids.size(); ++i) {
            ptr = append_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = append_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = append_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.length());
            ptr += msg.length();
            *ptr++ = '\n';
        }
        final_hash = compute_fnv1a(buffer.data(), static_cast<size_t>(ptr - buffer.data()));
    }

    return final_hash;
}
