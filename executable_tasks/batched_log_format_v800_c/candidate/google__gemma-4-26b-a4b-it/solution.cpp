#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion for unsigned integers
inline char* write_uint(char* ptr, uint32_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[10];
    int len = 0;
    while (val > 0) {
        buf[len++] = (val % 10) + '0';
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

inline char* write_uint16(char* ptr, uint16_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[5];
    int len = 0;
    while (val > 0) {
        buf[len++] = (val % 10) + '0';
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

inline void update_hash(uint64_t& hash, const char* start, const char* end) {
    for (const char* p = start; p < end; ++p) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
        hash *= 1099511628211ULL;
    }
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
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message lengths
    size_t total_max_size = 0;
    for (size_t i = 0; i < ids.size(); ++i) {
        total_max_size += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
    }

    std::vector<char> buffer(total_max_size);
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

        // Calculate hash for the current iteration's buffer
        uint64_t current_hash = 1469598103934665603ULL;
        update_hash(current_hash, buffer.data(), ptr);
        final_hash = current_hash;
    }

    return final_hash;
}