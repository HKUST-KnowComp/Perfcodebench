#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

namespace {

// Fast integer to string conversion for unsigned integers
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

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message lengths
    size_t max_line_size = 11 + 11 + 6 + 1 + 1; // ts|id|lvl|\n
    // First pass to find total size needed for the buffer
    size_t total_size = 0;
    for (size_t i = 0; i < ids.size(); ++i) {
        total_size += 11 + 11 + 6 + 1 + messages[i].size() + 1;
    }

    std::vector<char> buffer(total_size);
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
            for (char c : msg) {
                *ptr++ = c;
            }
            *ptr++ = '\n';
        }

        // FNV-1a hash calculation on the populated part of the buffer
        uint64_t hash = 1469598103934665603ULL;
        size_t bytes_written = ptr - buffer.data();
        for (size_t j = 0; j < bytes_written; ++j) {
            hash ^= static_cast<uint8_t>(buffer[j]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}