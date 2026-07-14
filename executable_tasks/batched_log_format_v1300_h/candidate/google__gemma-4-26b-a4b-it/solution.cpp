#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion writing directly to buffer
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
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and messages
    size_t max_line_size = 11 + 11 + 6 + 1 + 1; // ts|id|lvl|msg\n
    std::vector<char> buffer;
    buffer.reserve(ids.size() * 128); // Heuristic starting size

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        // We use a temporary vector of chars to build the string to avoid std::string overhead
        // but for simplicity and speed, we'll use a growing char vector.
        
        // To avoid frequent reallocations, we estimate size
        size_t estimated_size = 0;
        for(size_t i=0; i<ids.size(); ++i) {
            estimated_size += 30 + messages[i].size();
        }
        if (buffer.capacity() < estimated_size) buffer.reserve(estimated_size);

        for (std::size_t i = 0; i < ids.size(); ++i) {
            size_t start_idx = buffer.size();
            buffer.resize(start_idx + 64); // Reserve space for numbers
            char* ptr = &buffer[start_idx];

            ptr = write_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            size_t msg_len = messages[i].size();
            size_t current_pos = ptr - &buffer[0];
            buffer.resize(current_pos + msg_len + 1);
            std::memcpy(&buffer[current_pos], messages[i].data(), msg_len);
            ptr = &buffer[current_pos + msg_len];
            *ptr++ = '\n';

            // Adjust the actual size of the buffer used
            buffer.resize(ptr - &buffer[0]);
        }

        // Compute FNV-1a hash
        uint64_t hash = 1469598103934665603ULL;
        for (char c : buffer) {
            hash ^= static_cast<uint8_t>(c);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}