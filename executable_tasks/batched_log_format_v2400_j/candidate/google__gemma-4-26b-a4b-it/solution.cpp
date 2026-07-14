#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion that writes directly to a buffer
inline char* write_uint(char* ptr, uint32_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[10];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        *ptr++ = buf[--i];
    }
    return ptr;
}

inline char* write_uint16(char* ptr, uint16_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[5];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        *ptr++ = buf[--i];
    }
    return ptr;
}

inline uint64_t compute_fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < len; ++i) {
        hash ^= p[i];
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

    // Pre-calculate maximum possible size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and messages
    size_t max_line_size = 10 + 1 + 10 + 1 + 5 + 1 + 1024 + 1; // conservative estimate
    // Better: calculate exact size needed for one pass to be safe
    size_t total_capacity = 0;
    for (size_t i = 0; i < ids.size(); ++i) {
        total_capacity += 12 + 12 + 6 + messages[i].size() + 2;
    }

    std::vector<char> buffer(total_capacity);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (size_t i = 0; i < ids.size(); ++i) {
            ptr = write_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }
        final_hash = compute_fnv1a(buffer.data(), ptr - buffer.data());
    }

    return final_hash;
}